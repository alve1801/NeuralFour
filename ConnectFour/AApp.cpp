#include "stdafx.h"

#include "SysHelpers.h"




#include "AApp.h"
#include "DrawHelpers.h"
#include "Utils.h"

#include "Debugs.h"
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>


using namespace std;

boost::timer::cpu_timer FrameTimer;
boost::timer::cpu_timer TickTimer;


AApp::AApp() 
#if GARBAGE_CHECK
: GarbageDetector(this)
#endif
{
	bRunning = true;


	Mouse.Events.Add(UI::Mouse_Nothing);

	RenderData.Theme.BackGround = FColor(128);
	RenderData.Theme.MerelsLines = FColor(60);


	RestartIterations = 25;
	MerelsInstanceCount = 1;

}

AApp::~AApp()
{

}

int AApp::Execute()
{

	SDL_Event Event;
	InitSDL();
	Init();
	InitMainWindow();
	while (bRunning)
	{
		FrameTimer.start();

		while (SDL_PollEvent(&Event))
		{
			EventHandler(&Event);
		}

		Globals::DeltaTime = TIME_NS_TO_S(TickTimer.elapsed().wall);
		TickTimer.start();
		Tick();

		Mouse.Reset();
		Render();
		double FrameTime = TIME_NS_TO_MS(FrameTimer.elapsed().wall);
		if (FrameTime < MinimalFrameTime)
		{
			THREAD_SLEEP_MS(int(MinimalFrameTime - FrameTime));
		}
	}
	Destroy();

	return 0;
}

bool AApp::InitSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "Fatal Error: SDL_INIT_EVERYTHING failed: " << SDL_GetError() << endl;
		SDL_Delay(8000);
		return false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		cout << "Fatal Error: IMG_INIT_PNG failed: " << SDL_GetError() << endl;
		SDL_Delay(8000);
		return false;
	}

	if (TTF_Init() < 0)
	{
		//cout << "Fatal Error: TTF_Init failed: " << TTF_GetError() << endl;
		printf("TTF_Init: %s\n", TTF_GetError());
		SDL_Delay(8000);
		return false;
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	return true;
}

bool AApp::Init()
{
	Globals::ThreadInfos.reset(new AThreadInfo());

	if (!boost::filesystem::exists(SysHelpers::GetAppUserStorageLocation()))
	{
		boost::filesystem::create_directory(SysHelpers::GetAppUserStorageLocation());
	}
	
	srand(static_cast <unsigned> (time(nullptr)));

	RenderData.Fonts.Large = LoadFont("Fonts\\arial_unicode.ttf", 16);
	RenderData.Fonts.Medium = LoadFont("Fonts\\arial_unicode.ttf", 14);
	RenderData.Fonts.Small = LoadFont("Fonts\\arial_unicode.ttf", 12);

	return true;
}

bool AApp::InitMainWindow()
{
	MainWindow.Init(APP_NAME, FRectangle(1920/2, 32, 1920/2, 100));


	//MainWindow.SetWindowIcon("Textures\\Application Icon\\Icon_32.png");
	MainWindow.SetMinimumSize(FSize(640, 480));

	MainWindow.MainUserInterface.Init();
	MainWindow.SetUserInterface(&MainWindow.MainUserInterface);

	for (int Index = 0; Index < MerelsInstanceCount; ++Index)
	{
		Instances.push_back(new ACfInstance());
		Instances[Index]->Init();

		Players.push_back(Instances[Index]->Players[0]);
		Players.push_back(Instances[Index]->Players[1]);

		MainWindow.MainUserInterface.Graph->AddGraph();
		MainWindow.MainUserInterface.Graph->AddGraph();

		MainWindow.MainUserInterface.AddBoard(FRectangle(32 + (32+120) * Index, 32, 120, 120), Instances[Index]);
	}
	
	//MainWindow.MainUserInterface.Boards[0]->CfInstance = Instances[0];

	return true;
}


void AApp::EventHandler(SDL_Event* Event)
{
	switch (Event->type)
	{
	case SDL_QUIT:
		Exit();
		break;

	case SDL_WINDOWEVENT:
		switch (Event->window.event)
		{

		case SDL_WINDOWEVENT_CLOSE:
			break;

		default: 
			break;
		}
		break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEMOTION:
	case SDL_MOUSEWHEEL:
		MouseEventHandler(Event);
		break;

	case SDL_KEYDOWN:
		Keys.ProcessEvent(Event->key.keysym.sym, true);

		switch (Event->key.keysym.sym)
		{
		case SDLK_F1:
			bPaused = !bPaused;
			break;

		case SDLK_F2:
			bDrawBoards = !bDrawBoards;
			break;
		case SDLK_F10:
			EndLearingPhase();
			break;

		

		default:
			break;
		}
		break;
	}
	
}

void AApp::MouseEventHandler(SDL_Event* Event)
{
	if (Event->type == SDL_MOUSEMOTION)
	{
		Mouse.Events.Add(UI::Mouse_Move);

		Mouse.LastPosition = Mouse.Position;
		Mouse.Position.x = Event->button.x;
		Mouse.Position.y = Event->button.y;
		Mouse.CurrentDrag = Mouse.Position - Mouse.LastPosition;

	}

	else if (Event->type == SDL_MOUSEBUTTONDOWN)
	{
		switch (int(Event->button.button))
		{
		case 1:


			Mouse.Events.Add(UI::Mouse_LClick);

			Mouse.bDrag = true;

			break;
		case 2:
			break;
		case 3:

			Mouse.Events.Add(UI::Mouse_RClick);

			Mouse.bDrag = true;
			break;
		default:
			break;
		}
	}

	else if (Event->type == SDL_MOUSEBUTTONUP)
	{
		switch (int(Event->button.button))
		{
		case 1:

			Mouse.Events.Remove(UI::Mouse_LDown);
			Mouse.Events.Add(UI::Mouse_LUp);


			break;
		case 2:
			break;
		case 3:

			Mouse.Events.Remove(UI::Mouse_RDown);
			Mouse.Events.Add(UI::Mouse_RUp);

			break;
		default:
			break;
		}
	}
	

	else if (Event->type == SDL_MOUSEWHEEL)
	{
		Mouse.Events.Add(Event->wheel.y > 0 ? UI::Mouse_WUp : UI::Mouse_WDown);
	}

}


void AApp::EndLearingPhase()
{
	/*Globals::bLearningPhase = false;
	Globals::MerelsArchive = MerelsArchives[0];
	double Progress = 0;
	double TotalProgress = 0;
	bool bDone = false;
	
	boost::thread MergeThread(boost::bind(&AApp::MergeArchives, this, &bDone, &Progress, &TotalProgress));

	while (!bDone)
	{
		PRINT "Merging Global Archive. Progress[%]: " << (Progress + TotalProgress)/ MerelsInstanceCount * 100 END;
	}
	PRINT "Global Archive Merged!" END;*/

}

void AApp::Tick()
{

	MainWindow.Tick();

	ULong TotalEntries = 0;
	ULong Size = 0;

	bool bNextGeneration = true;
	for (int Index = 0; Index < Instances.size(); ++Index)
	{
		//ASmartWriteLock Lock(Instances[Index]->Mutex);

		if (!Instances[Index]->bNextGeneration)
		{
			bNextGeneration = false;
			break;
		}

	}
	//if (bNextGeneration)
	//{
	//	vector<ASmartWriteLock*> Locks;

	//	vector<NeuralNetwork::ASharedInstance> Neuralnetworks;

	//	for (int Index = 0; Index < Instances.size(); ++Index)
	//	{

	//		//static ULong BaseValue =

	//		//MainWindow.MainUserInterface.Graph->AddValue(Index*2, Instances[Index]->Players[0]->NeuralNetworkInstance->TotalFitness);
	//		//MainWindow.MainUserInterface.Graph->AddValue(Index*2+1, Instances[Index]->Players[1]->NeuralNetworkInstance->TotalFitness);
	//		Locks.push_back(new ASmartWriteLock(Instances[Index]->Mutex));

	//		Neuralnetworks.push_back(Instances[Index]->Players[0]->NeuralNetworkInstance);
	//		Neuralnetworks.push_back(Instances[Index]->Players[1]->NeuralNetworkInstance);
	//	}


	//	//for (int Index = 0; Index < Neuralnetworks.size(); ++Index)
	//	//{
	//	//	Neuralnetworks[Index]->GenerateDna();
	//	//}
	//	//Generations++;

	//	//std::sort(Neuralnetworks.begin(), Neuralnetworks.end(), [](NeuralNetwork::ASharedInstance a, NeuralNetwork::ASharedInstance b)
	//	//{
	//	//	return a->Fitness > b->Fitness;
	//	//});

	//	//size_t Length = Neuralnetworks.size();
	//	//size_t Suvivors = Length / 2;

	//	//for (int Index = 0; Index < Suvivors-1; ++Index)
	//	//{
	//	//	Neuralnetworks[Index + Suvivors]->GenerateOffspring(Players[Index]->NeuralNetworkInstance, Players[Index + 1]->NeuralNetworkInstance);
	//	//	Neuralnetworks[Index]->Mutate();
	//	//	Neuralnetworks[Index + Suvivors]->Mutate();
	//	//}
	//	//Neuralnetworks[Suvivors - 1]->Mutate();

	//	//Neuralnetworks[Length - 1]->GenerateOffspring(Players[0]->NeuralNetworkInstance, Players[Suvivors - 1]->NeuralNetworkInstance);
	//	//Neuralnetworks[Length-1]->Mutate();

	//	///*for (size_t Index = 0; Index < Instances.size(); ++Index)
	//	//{
	//	//	Instances[Index]->Players[0]->NeuralNetworkInstance = Neuralnetworks[Index * 2];
	//	//	Instances[Index]->Players[1]->NeuralNetworkInstance = Neuralnetworks[Index * 2 + 1];
	//	//}*/

	//	for (size_t Index = 0; Index < Instances.size(); ++Index)
	//	{
	//		Instances[Index]->bNextGeneration = false;
	//	}


	//	while (!Locks.empty())
	//	{
	//		delete Locks.back();
	//		Locks.pop_back();
	//	}

	//	

	//	PRINT "Next Gen" TAB Generations END;
	//}

	//MainWindow.MainUserInterface.Graph->TotalEntries.push_back(Globals::MerelsArchive.GetEntryCount());

}


void AApp::Render()
{
	//ASmartReadLock MerelsArchiveLock(Globals::MerelsArchive.Mutex);
	MainWindow.Render();
}


void AApp::Destroy()
{
	MainWindow.Destroy();
	SDL_Quit();
}

void AApp::Exit()
{
	bRunning = false;
	/*for (ACfArchive* Archive : MerelsArchives)
	{
		Archive->Close();
	}*/
}



