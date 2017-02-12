#include "stdafx.h"
#include "AUserInterface.h"
#include "DrawHelpers.h"
#include "AApp.h"
#include "Globals.h"

using namespace std;



void UI::FButtonIcons::UpdateTints(FColor NewDefaultTint, FColor NewHoveredTint, FColor NewActivatedTint)
{
	HoveredTint = NewHoveredTint;
	ActivatedTint = NewActivatedTint;

	if (DefaultIcon)
	{
		SetTextureTintAlpha(DefaultIcon, NewDefaultTint);
	}
	if (HoveredIcon)
	{
		SetTextureTintAlpha(HoveredIcon, NewHoveredTint);
	}
	if (ActivatedIcon)
	{
		SetTextureTintAlpha(ActivatedIcon, NewActivatedTint);
	}
}

void UI::FButtonIcons::UpdateAlpha(UChar NewDefaultAlpha, UChar NewHoveredAlpha, UChar NewActivatedAlpha)
{
	HoveredTint.A = NewHoveredAlpha;
	ActivatedTint.A = NewActivatedAlpha;

	if (DefaultIcon)
	{
		SetTextureAlpha(DefaultIcon, NewDefaultAlpha);
	}
	if (HoveredIcon)
	{
		SetTextureAlpha(HoveredIcon, NewHoveredAlpha);
	}
	if (ActivatedIcon)
	{
		SetTextureAlpha(ActivatedIcon, NewActivatedAlpha);
	}
}

void UI::FCheckboxIcons::UpdateTints(FColor NewDefaultTint, FColor NewHoveredTint, FColor NewActivatedTint, FColor NewBorderColor)
{
	Checked.UpdateTints(NewDefaultTint, NewHoveredTint, NewActivatedTint);
	UnChecked.UpdateTints(NewDefaultTint, NewHoveredTint, NewActivatedTint);
	BorderColor = NewBorderColor;
}

void UI::FCheckboxIcons::UpdateAlpha(UChar NewDefaultAlpha, UChar NewHoveredTintAlpha, UChar NewActivatedTintAlpha, UChar NewBorderAlpha)
{
	Checked.UpdateAlpha(NewDefaultAlpha, NewHoveredTintAlpha, NewActivatedTintAlpha);
	UnChecked.UpdateAlpha(NewDefaultAlpha, NewHoveredTintAlpha, NewActivatedTintAlpha);
	BorderColor.A = NewBorderAlpha;
}



UI::AWindow::AWindow()
{
}

UI::AWindow::~AWindow()
{
}

void UI::AWindow::SetUserInterface(AUserInterface* NewUserInterface)
{
	RETURN_IF_NULLPTR(NewUserInterface);
	UserInterface = NewUserInterface;
	UserInterface->ParentWindowAreaUpdate();
	HoveredComponent = nullptr;
	bBlockUserInterfaceInput = false;
}

void UI::AWindow::SetWindowIcon(string Path) const
{

	SDL_Surface* WindowIcon;
	SDL_RWops* rwop;
	rwop = SDL_RWFromFile(Path.c_str(), "rb");
	WindowIcon = IMG_LoadPNG_RW(rwop);
	if (WindowIcon)
	{
		SDL_SetWindowIcon(Window, WindowIcon);

	}
	else
	{
		printf("IMG_LoadICO_RW: %s\n", IMG_GetError());
	}
	SDL_FreeRW(rwop);
}

void UI::AWindow::SetWindowTitle(string NewTitle) const
{
	SDL_SetWindowTitle(Window, NewTitle.c_str());
}

UI::FTexture* UI::AWindow::LoadTexture(string Path)
{
	SDL_Texture* SDLTexture = IMG_LoadTexture(Renderer, Path.c_str());
	if (!SDLTexture)
	{
#if DRAW_SAFE
		PRINT IMG_GetError() END;
#endif
		return nullptr;
	}
	FTexture* Texture = new FTexture(SDLTexture, Path);
	Textures.push_back(Texture);
	return Texture;
}

void UI::AWindow::Init(string Name, FRectangle& InArea, FFlag<UShort>* InPtrMouseEvents, FPoint* InPtrMousePosition, UInt* WindowFlags, UInt* RendererFlags)
{

	PtrMouseEvents = InPtrMouseEvents;
	PtrMousePosition = InPtrMousePosition;

	Window = SDL_CreateWindow(Name.c_str(), InArea.TL.x, InArea.TL.y, InArea.GetWidth(), InArea.GetHeight(), WindowFlags ? *WindowFlags : SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (Window == nullptr)
	{
		cout << "Fatal Error: SDL_CreateWindow failed: " << SDL_GetError() << endl;
		SDL_Delay(8000);
	}

	Renderer = SDL_CreateRenderer(Window, -1, RendererFlags ? *RendererFlags : SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Renderer == nullptr)
	{
		SDL_DestroyWindow(Window);
		cout << "Fatal Error: SDL_CreateRenderer failed: " << SDL_GetError() << endl;
	}

	Area = InArea;

	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);

	bGhostWindowDimensionsChanged = true;
}

void UI::AWindow::Destroy()
{
	//SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
}

void UI::AWindow::Render()
{
	SDL_RenderClear(Renderer);

	if (UserInterface)
	{
		UserInterface->Render(Renderer);
	}
	SDL_RenderPresent(Renderer);
}

void UI::AWindow::Tick()
{
	WindowFlags = SDL_GetWindowFlags(Window);

	MouseEvents = *PtrMouseEvents;
	MousePosition = *PtrMousePosition;

	if (WindowFlags & SDL_WINDOW_MOUSE_FOCUS || bGhostWindowDimensionsChanged)
	{
		bIsFocussed = true;
	}
	else
	{
		bIsFocussed = false;
	}

	if ((bIsFocussed || bTickAlways) && UserInterface)
	{
		UserInterface->Tick();
	}

	FPoint Dimensions, Position;
	SDL_GetWindowSize(Window, &Dimensions.x, &Dimensions.y);
	bGhostWindowDimensionsChanged = false;

	if (Dimensions != Area.GetDimensions())
	{
		bWindowDimensionsChanged = bGhostWindowDimensionsChanged = true;
		Area.SetDimensions(Dimensions);
		if (UserInterface)
		{
			UserInterface->ParentWindowAreaUpdate();
		}
	}
	SDL_GetWindowPosition(Window, &Position.x, &Position.y);
	if (Position != Area.TL)
	{
		Area.SetAnchor(Position);
	}
}





UI::AUserInterface::AUserInterface(AWindow* InParentWindow) : AContainer(this, FRectangle()), ParentWindow(InParentWindow)
{
	/*MouseEvents = InMouseEvents;
	MousePosition = InMousePosition;*/
}

UI::AUserInterface::~AUserInterface()
{
}

void UI::AUserInterface::Tick()
{
	AContainer::Tick();
}

void UI::AUserInterface::Render(SDL_Renderer* Renderer)
{
	AContainer::Render(Renderer);
}

void UI::AUserInterface::Init()
{
	Area = ParentWindow->GetArea();
}





UI::AComponent::AComponent(AUserInterface* InParentUserInterface, FRectangle InArea) : ParentUserInterface(InParentUserInterface), ParentContainer(nullptr)
{
	Area = InArea;
}

UI::AComponent::~AComponent()
{
	if (ParentContainer)
	{
		ParentContainer->RemoveComponent(this);
	}
	ParentUserInterface->ResetHoveredComponent(this);
}

void UI::AComponent::Render(SDL_Renderer * Renderer)
{

}

void UI::AComponent::Tick()
{

}

void UI::AComponent::SetDimensions(FPoint NewDimensions)
{
	Area.SetDimensions(NewDimensions);
	if (ParentContainer) ParentContainer->UpdateComponents();
}

void UI::AComponent::SetAnchor(FPoint NewAnchor)
{
	Area.SetAnchor(NewAnchor);
	if (ParentContainer) ParentContainer->UpdateComponents();
}

void UI::AComponent::ChangeAnchor(FPoint DeltaAnchor)
{
	SetAnchor(Area.TL + DeltaAnchor);
	if (ParentContainer) ParentContainer->UpdateComponents();
}

void UI::AComponent::SetHorizontalAnchor(int x)
{
	ChangeAnchor(FPoint(x - Area.TL.x, 0));
	if (ParentContainer) ParentContainer->UpdateComponents();
}

void UI::AComponent::SetVerticalAnchor(int y)
{
	ChangeAnchor(FPoint(0, y - Area.TL.y));
	if (ParentContainer) ParentContainer->UpdateComponents();
}




UI::AContainer::AContainer(AUserInterface* InParentUserInterface, FRectangle InArea) : AComponent(InParentUserInterface, InArea)
{

}

UI::AContainer::~AContainer()
{
}
	
void UI::AContainer::Tick()
{
	for (auto Component : Components)
	{
		if (Component->Visibility.Has(UI_Tick))
		{
			Component->Tick();
		}
	}
	

	if (bUpdatePositions)
	{
		UpdatePositions();
		bUpdatePositions = false;
	}
}

void UI::AContainer::Render(SDL_Renderer* Renderer)
{
	for (auto Component : Components)
	{
		if (Component->Visibility.Has(UI_Render))
		{
			Component->Render(Renderer);
		}
	}
}

void UI::AContainer::SetAnchor(FPoint NewAnchor)
{
	ChangeAnchor(NewAnchor - Area.TL);
}

void UI::AContainer::ChangeAnchor(FPoint DeltaAnchor)
{
	for each (AComponent* Component in Components)
	{
		if (Component->Flags.Has(UI_Parented))
		{
			continue;
		}
		Component->ChangeAnchor(DeltaAnchor);
	}
	AComponent::SetAnchor(Area.TL + DeltaAnchor);;
}

UI::AComponent* UI::AContainer::AddComponent(AComponent* Component, int Index)
{
	if (Index < 0)
	{
		Index = max(int(Components.size()) + Index + 1, 0);
	}
	Components.insert(Components.begin() + Index, Component);
	Component->ParentContainer = this;
	bUpdatePositions = true;
	return Component;
}

void UI::AContainer::RemoveComponent(AComponent* Component)
{
	for (size_t Index = 0; Index < Components.size(); Index++)
	{
		if (Component == Components[Index])
		{
			Components.erase(Components.begin() + Index);
			break;
		}
	}
}

void UI::AContainer::UpdatePositions()
{
	if (Flags.Has(UI_HorizontalBox))
	{
		int x = Area.TL.x;
		for (auto Component : Components)
		{
			if (Component->Flags.Has(UI_Parented))
			{
				continue;
			}
			x += Component->Padding.TL.x;
			Component->SetHorizontalAnchor(x);
			x += Component->Area.GetWidth() + Component->Padding.BR.x;
		}
	}
	if (Flags.Has(UI_VerticalBox))
	{
		int y = Area.TL.y;
		for (auto Component : Components)
		{
			if (Component->Flags.Has(UI_Parented))
			{
				continue;
			}
			y += Component->Padding.TL.y;
			Component->SetVerticalAnchor(y);
			y += Component->Area.GetHeight() + Component->Padding.BR.y;
		}
	}

	if (Flags.Has(UI_AutoArea))
	{
		bool bFirst = true;
		bool bAutoAnchor = Flags.Has(UI_AutoAnchor);
		bool bAutoSize = Flags.Has(UI_AutoSize);
		for (auto Component : Components)
		{
			if (Component->Flags.Has(UI_Parented))
			{
				continue;
			}
			FRectangle ComponentArea = Component->GetPaddedArea();
			if (bFirst)
			{
				bFirst = false;
				Area.BR = ComponentArea.BR;
			}
			else
			{

				if (bAutoAnchor)
				{
					if (ComponentArea.TL.x < Area.TL.x)
					{
						Area.TL.x = ComponentArea.TL.x;
					}

					if (ComponentArea.TL.y < Area.TL.y)
					{
						Area.TL.y = ComponentArea.TL.y;
					}
				}
				if (bAutoSize)
				{
					if (ComponentArea.BR.x > Area.BR.x)
					{
						Area.BR.x = ComponentArea.BR.x;
					}

					if (ComponentArea.BR.y > Area.BR.y)
					{
						Area.BR.y = ComponentArea.BR.y;
					}
				}
			}
		}
	}
}


UI::AImage::AImage(AUserInterface* InParentUserInterface, FRectangle InArea, FTexture* InTexture, float InAngle)
	: AComponent(InParentUserInterface, InArea), Texture(InTexture), Angle(InAngle)
{
}

UI::AImage::~AImage()
{
}

void UI::AImage::Render(SDL_Renderer* Renderer)
{
	DrawTexture(Renderer, Texture, Area.TL, Angle, Area.GetDimensions());
}



UI::AText::AText(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, wstring InText, FColor InColor, float InOffset)
	: AComponent(InParentUserInterface, InArea), Font(InFont), Color(InColor), Offset(InOffset)
{
	if (InOffset != 1.f)
	{
		Flags.Add(UI_AutoArea);
	}

	Flags.Add(UI_AutoSize);

	SetText(InText);
}

UI::AText::AText(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, wstring* InTextPtr, FColor InColor, float InOffset)
	: AText(InParentUserInterface, InArea, InFont, *InTextPtr, InColor, InOffset)
{
	TextPtr = InTextPtr;
}

UI::AText::~AText()
{
}

void UI::AText::Tick()
{
	bTextWasChanged = false;
	if (TextPtr && *TextPtr != Text)
	{
		SetText(*TextPtr);
	}
}

void UI::AText::Render(SDL_Renderer* Renderer)
{
	DrawFont(Renderer, Area.TL, Text, Font, Color, bMultiLine, ETextDrawType::Blended);
}

void UI::AText::SetTextPointer(wstring* NewTextPointer)
{
	TextPtr = NewTextPointer;
	SetText(*TextPtr);
}

void UI::AText::SetText(wstring NewText)
{
	if (Text == NewText)
	{
		return;
	}
	Text = NewText;

	int OldWidth = 0;
	if (Flags.Has(UI_AutoSize) && Font)
	{
		OldWidth = Area.GetWidth();
		FSize Size;
		TTF_SizeUNICODE(Font, (UShort*) Text.data(), &Size.x, &Size.y);

		if (bMultiLine)
		{
			UShort Lines = 1;

			for (wchar_t SCharacter : Text)
			{
				if (SCharacter == '\n')
				{
					Lines++;
				}
			}
			Size.y *= Lines;
		}

		SetDimensions(Size);

		if (ParentContainer) ParentContainer->UpdateComponents();
	}
	if (Flags.Has(UI_AutoArea) && Offset != 1.f)
	{
		int Delta = int((Area.GetWidth() - OldWidth) * (1 - (Offset + 1)/2));
		ChangeAnchor(FPoint(-Delta, 0));
		if (ParentContainer) ParentContainer->UpdateComponents();

	}
	bTextWasChanged = true;

}





UI::AButton::AButton(AUserInterface* InParentUserInterface, FRectangle InArea, FButtonIcons InIcons) : AComponent(InParentUserInterface, InArea), TextComponent(nullptr)
{
	Icons = InIcons;
	Icon = Icons.DefaultIcon;
	DrawArea = InArea;
}

UI::AButton::~AButton()
{
	if (TextComponent)
	{
		delete TextComponent;
	}
}

UI::AButton* UI::AButton::AddEventCallback(FFlag<UShort> ButtonEvent, FCallback CallbackData)
{
	Callbacks[ButtonEvent.GetFlag()] = CallbackData;
	return this;
}

UI::AButton* UI::AButton::AddEventCallback(FFlag<UShort> ButtonEvent, boost::function<void()> Callback, bool bTriggerOnActivated)
{
	Callbacks[ButtonEvent.GetFlag()] = FCallback(Callback);
	return this;
}

UI::AButton* UI::AButton::AddEvent(FFlag<UShort> ButtonEvent, bool bTriggerOnActivated)
{
	Callbacks[ButtonEvent.GetFlag()] = FCallback(bTriggerOnActivated);
	return this;
}

void UI::AButton::Tick()
{
	if (Area.Contains(ParentUserInterface->ParentWindow->MousePosition) && ParentUserInterface->SetHoveredComponent(this))
	{
		bIsActivated = false;

		for (auto It : Callbacks)
		{
			if (ParentUserInterface->ParentWindow->MouseEvents.Has(It.first) && (!(It.first & Button_Hover) || !bIsHovered))
			{
				bIsActivated = true;

				if (It.second.bTriggerOnActivated)
				{
					OnActivated(true);
				}
				if (It.second.Callback)
				{
					It.second.Callback();
				}
			}
		}
		bIsHovered = true;

		if (!Icons.bOverlay && !Icons.bDefaultReactions)
		{
			if (!bIsActivated)
			{
				Icon = Icons.HoveredIcon;
			}
			else
			{
				Icon = Icons.ActivatedIcon;
			}
		}
		
	}
	else if (bIsHovered)
	{
		ParentUserInterface->ResetHoveredComponent(this);
		bIsHovered = bIsActivated = false;
		if (!Icons.bOverlay && !Icons.bDefaultReactions)
		{
			Icon = Icons.DefaultIcon;
		}
	}

	if (TextComponent)
	{
		if (TextComponent->GetTextWasChanged())
		{
			Area.TL.x = (TextComponent->GetArea().TL.x < Area.TL.x) ? TextComponent->GetArea().TL.x : Area.TL.x;
			//Area.TL.y = (TextComponent->GetArea().TL.y < Area.TL.y) ? TextComponent->GetArea().TL.y : Area.TL.y;

			Area.BR.x = (TextComponent->GetArea().BR.x > Area.BR.x) ? TextComponent->GetArea().BR.x : Area.BR.x;
			//Area.BR.y = (TextComponent->GetArea().BR.y > Area.BR.y) ? TextComponent->GetArea().BR.y : Area.BR.y;


			if (bSizeToText)
			{
				IconArea.TL.x = (TextComponent->GetArea().TL.x < IconArea.TL.x) ? TextComponent->GetArea().TL.x : IconArea.TL.x;
				//Area.TL.y = (TextComponent->GetArea().TL.y < Area.TL.y) ? TextComponent->GetArea().TL.y : Area.TL.y;

				IconArea.BR.x = (TextComponent->GetArea().BR.x > IconArea.BR.x) ? TextComponent->GetArea().BR.x : IconArea.BR.x;

			}
			if (ParentContainer) ParentContainer->UpdateComponents();

		}
		TextComponent->Tick();	
	}
}

void UI::AButton::Render(SDL_Renderer* Renderer)
{
	if (Icons.bDefaultReactions)
	{
		if (Icons.bTintBackground)
		{
			if (bIsActivated)
			{
				SetRenderColor(Renderer, Icons.ActivatedTint);
				DrawRectangle(Renderer, DrawArea);
			}
			else if (bIsHovered)
			{
				SetRenderColor(Renderer, Icons.HoveredTint);
				DrawRectangle(Renderer, DrawArea);
			}
		}
		else
		{
			if (bIsActivated)
			{
				SetTextureTint(Icon, Icons.ActivatedTint);
				DrawTexture(Renderer, Icon, DrawArea.TL, 0, DrawArea.GetDimensions());
			}
			else if (bIsHovered)
			{
				SetTextureTint(Icon, Icons.HoveredTint);
				DrawTexture(Renderer, Icon, DrawArea.TL, 0, DrawArea.GetDimensions());
			}
		}
	}

	if (Icons.bOverlay && !Icons.bDefaultReactions || Icons.bDefaultReactions && Icons.bTintBackground && Icons.DefaultIcon)
	{
		DrawTexture(Renderer, Icons.DefaultIcon, DrawArea.TL, 0, DrawArea.GetDimensions());
	}
	else if (Icon)
	{
		DrawTexture(Renderer, Icon, DrawArea.TL, 0, DrawArea.GetDimensions());
	}


	if (!Icons.bDefaultReactions)
	{
		if (bIsActivated)
		{
			DrawTexture(Renderer, Icons.ActivatedIcon, DrawArea.TL, 0, DrawArea.GetDimensions());
		}
		else if (bIsHovered)
		{
			DrawTexture(Renderer, Icons.HoveredIcon, DrawArea.TL, 0, DrawArea.GetDimensions());
		}
	}

	if (TextComponent)
	{
		TextComponent->Render(Renderer);
	}
}

void UI::AButton::AddTextComponent(AText* NewTextComponent)
{
	NewTextComponent->Flags.Add(UI_Parented);
	TextComponent = NewTextComponent;
	Flags.Add(UI_AutoSize);
	OnTextAdded();
}

void UI::AButton::AddTextComponent(FPoint Offset, TTF_Font* InFont, wstring InText, FColor InColor, float TextCenterOffset)
{
	AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(Offset)), InFont, InText, InColor, TextCenterOffset));
}

void UI::AButton::AddTextComponent(FPoint Offset, TTF_Font* InFont, wstring* InTextPtr, FColor InColor, float TextCenterOffset)
{
	AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(Offset)), InFont, InTextPtr, InColor, TextCenterOffset));
}

void UI::AButton::OnActivated(const bool Callback)
{

}




UI::ACheckbox::ACheckbox(AUserInterface* InParentUserInterface, FRectangle InArea, FCheckboxIcons InCheckboxIcons, const bool InbIsChecked)
	: AButton(InParentUserInterface, InArea, InbIsChecked ? InCheckboxIcons.Checked : InCheckboxIcons.UnChecked)
{
	bIsChecked = InbIsChecked;

	CheckboxIcons = InCheckboxIcons;
}

UI::ACheckbox::~ACheckbox()
{
}

void UI::ACheckbox::SetIsChecked(const bool NewIsChecked)
{
	bIsChecked = NewIsChecked;
	Icons = bIsChecked ? CheckboxIcons.Checked : CheckboxIcons.UnChecked;
	UpdateIcon();
}

void UI::ACheckbox::Tick()
{
	AButton::Tick();
}

void UI::ACheckbox::Render(SDL_Renderer * Renderer)
{

	if (CheckboxIcons.bDefaultCheckbox)
	{
		SetRenderColor(Renderer, CheckboxIcons.BorderColor);

		DrawRectangle(Renderer, DrawArea, CheckboxIcons.BorderThickness);

		if (bIsChecked)
		{
			DrawRectangle(Renderer, FRectangle(DrawArea.TL + FPoint(CheckboxIcons.BorderThickness+1), DrawArea.BR - FPoint(CheckboxIcons.BorderThickness+1)), DRAW_FILLED);
		}
		else if (bIsActivated)
		{
			SetRenderColor(Renderer, CheckboxIcons.Checked.ActivatedTint);

			DrawRectangle(Renderer, FRectangle(DrawArea.TL + FPoint(CheckboxIcons.BorderThickness + 1), DrawArea.BR - FPoint(CheckboxIcons.BorderThickness + 1)), DRAW_FILLED);

		}
		else if (bIsHovered)
		{
			SetRenderColor(Renderer, CheckboxIcons.Checked.HoveredTint);

			DrawRectangle(Renderer, FRectangle(DrawArea.TL + FPoint(CheckboxIcons.BorderThickness + 1), DrawArea.BR - FPoint(CheckboxIcons.BorderThickness + 1)), DRAW_FILLED);

		}



		if (TextComponent)
		{
			TextComponent->Render(Renderer);
		}
	}
	else
	{
		if (Icons.bDefaultReactions && bIsChecked)
		{
			SetRenderColor(Renderer, CheckboxIcons.BorderColor);
			DrawRectangle(Renderer, DrawArea, CheckboxIcons.BorderThickness);
		}
		AButton::Render(Renderer);
	}
}

void UI::ACheckbox::OnActivated(const bool Callback)
{
	if (Callback)
	{
		if (!bIsChecked)
		{
			SetIsChecked(true);
		}
		else if (bAllowDeactivate)
		{
			SetIsChecked(false);
		}
	}
}




UI::ADragHandle::ADragHandle(AUserInterface* InParentUserInterface, FRectangle InArea, FColor InColor, bool InbIsHorizontal, int InDragMin, int InDragMax) : AComponent(InParentUserInterface, InArea), ControlledComponent(nullptr)
{
	Color = InColor;
	bIsHorizontal = InbIsHorizontal;
	DragMin = InDragMin;
	DragMax = InDragMax;
	ComponentOffset = bIsHorizontal ? Area.GetWidth() / 2 : Area.GetHeight() / 2;
}

UI::ADragHandle::~ADragHandle()
{
}

void UI::ADragHandle::Tick()
{

	if (Area.Contains(ParentUserInterface->ParentWindow->MousePosition) && ParentUserInterface->SetHoveredComponent(this))
	{
		bIsHovered = true;

		if (ParentUserInterface->ParentWindow->MouseEvents.Has(StartEvents))
		{
			bIsActivated = true;
		}
	}
	else if (bIsHovered)
	{
		ParentUserInterface->ResetHoveredComponent(this);

		bIsHovered = false;
	}
	if (bIsActivated)
	{
		if (ParentUserInterface->ParentWindow->MouseEvents.Has(EndEvents))
		{
			bIsHovered = bIsActivated = false;

		}


		int NewPosition;

		if (bIsHorizontal)
		{
			NewPosition = ParentUserInterface->ParentWindow->MousePosition.x;
		}
		else
		{
			NewPosition = ParentUserInterface->ParentWindow->MousePosition.y;
		}

		SetPosition(NewPosition);

	}
	else
	{
		int NewPosition;

		if (bIsHorizontal)
		{
			NewPosition = ControlledComponent->GetArea().TL.x - ComponentOffset;
		}
		else
		{
			NewPosition = ControlledComponent->GetArea().TL.y - ComponentOffset;
		}
		if (NewPosition != Position)
		{
			SetPosition(NewPosition);
		}
	}
	AComponent::Tick();
}

void UI::ADragHandle::Render(SDL_Renderer* Renderer)
{
	SetRenderColor(Renderer, Color);
	UChar Offset = 0;
	int Max = ((Area.BR.x - Area.TL.x) % 2 == 0) ? Area.BR.x - 2 : Area.BR.x;
	for (int x = Area.TL.x; x < Max; x += 2)
	{
		for (int y = Area.TL.y + Offset; y < Area.BR.y - Offset; y += 4)
		{
			SDL_RenderDrawPoint(Renderer, x, y);
		}
		Offset = Offset ? 0 : 2;
	}
}

void UI::ADragHandle::SetPosition(int NewPosition)
{
	int HalfThickness;
	if (bIsHorizontal)
	{
		HalfThickness = Area.GetDimensions().x / 2;
		Position = Utils::Clamp(NewPosition, DragMin + HalfThickness, DragMax - HalfThickness);
		SetHorizontalAnchor(Position - Area.GetDimensions().x / 2);
		ControlledComponent->SetHorizontalAnchor(Position + ComponentOffset);
	}
	else
	{
		HalfThickness = Area.GetDimensions().y / 2;
		Position = Utils::Clamp(NewPosition, DragMin + HalfThickness, DragMax - HalfThickness);
		SetVerticalAnchor(Position - HalfThickness);
		ControlledComponent->SetVerticalAnchor(Position + ComponentOffset);
	}

	bWasChanged = true;
}




UI::ASlider::ASlider(AUserInterface* InParentUserInterface, FRectangle InArea, FPoint InButtonSize, FSliderIcons InIcons, float InPrecentage)
	: AComponent(InParentUserInterface, InArea), Button(new AButton(InParentUserInterface, InArea, InIcons.Background)), Percentage(InPrecentage)
{
	IconArea = FRectangle(FPoint(InArea.TL.x, (Area.TL.y + Area.BR.y) / 2 - InButtonSize.y / 2), InButtonSize.x, InButtonSize.y);

	SetPercentage(InPrecentage);
	Icons = InIcons;
}

UI::ASlider::~ASlider()
{
	delete Button;
}

void UI::ASlider::Tick()
{
	Button->Tick();
	if (Button->bIsActivated)
	{
		bDragged = true;
		float NewPrecentage = float(ParentUserInterface->ParentWindow->MousePosition.x - Area.TL.x - IconArea.GetWidth() / 2) / (Area.BR.x - Area.TL.x - IconArea.GetWidth());
		SetPercentage(NewPrecentage);
	}
	else
	{
		bDragged = false;
	}
}

void UI::ASlider::Render(SDL_Renderer* Renderer)
{
	DrawTexture(Renderer, Icons.Background, Area.TL, 0, Area.GetDimensions());

	DrawTexture(Renderer, Button->bIsHovered ? Icons.Button.HoveredIcon : (Button->bIsActivated ? Icons.Button.ActivatedIcon : Icons.Button.DefaultIcon), IconArea.TL, 0, IconArea.GetDimensions());
}



UI::ASingleChoiceBox::ASingleChoiceBox(AUserInterface* InParentUserInterface, FRectangle InArea) : AContainer(InParentUserInterface, InArea)
{
}

UI::ASingleChoiceBox::~ASingleChoiceBox()
{
}

void UI::ASingleChoiceBox::Tick()
{
	AContainer::Tick();
	for each (ACheckbox* CurrentCheckbox in Components)
	{
		CurrentCheckbox->bAllowDeactivate = bAllowDeactivate;
		if (CurrentCheckbox->GetIsChecked())
		{
			if (CurrentCheckbox != CurrentCheckedBox)
			{
				if (CurrentCheckedBox != NULL)
				{
					CurrentCheckedBox->SetIsChecked(false);
				}
				CurrentCheckedBox = CurrentCheckbox;
			}

		}
	}

}

void UI::ASingleChoiceBox::SetAllowDeactivate(bool NewAllowDeactivate)
{
	if (bAllowDeactivate == NewAllowDeactivate)
	{
		return;
	}
	bAllowDeactivate = NewAllowDeactivate;
	for each (ACheckbox* CurrentCheckbox in Components)
	{
		CurrentCheckbox->bAllowDeactivate = bAllowDeactivate;
	}
}

UI::ACheckbox* UI::ASingleChoiceBox::AddComponent(ACheckbox* Component, int Index)
{
	Component->bAllowDeactivate = bAllowDeactivate;
	AContainer::AddComponent(Component, Index);
	return Component;
}




UI::ADropDownMenu::ADropDownMenu(AUserInterface* InParentUserInterface, FPoint InEntryDimensions, FColor InBackgroundColor, FColor InBorderColor) : AContainer(InParentUserInterface, FRectangle(0, 0, InEntryDimensions.x, 0))
{
	EntryDimensions = InEntryDimensions;
	BorderColor = InBorderColor;
	BackgroundColor = InBackgroundColor;
	Visibility = UI_Hidden;
	Flags.Add(UI_VerticalBox | UI_AutoArea);
}

UI::ADropDownMenu::~ADropDownMenu()
{
}

void UI::ADropDownMenu::Tick()
{
	AContainer::Tick();

	if (bIsOpen && (Area.Contains(ParentUserInterface->ParentWindow->MousePosition) || ParentUserInterface->ParentWindow->MousePosition.GetDistance<int>(Area.TL) < AutoCloseDistance))
	{
		bIsHovered = true;
	}
	else
	{
		bIsHovered = false;
		if (bIsOpen && bAutoClose)
		{
			Close();
		}
	}

}

void UI::ADropDownMenu::Render(SDL_Renderer* Renderer)
{
	SetRenderColor(Renderer, BackgroundColor);
	DrawRectangle(Renderer, Area, DRAW_FILLED);

	SetRenderColor(Renderer, BorderColor);
	DrawRectangle(Renderer, Area, 1);
	AContainer::Render(Renderer);
}

UI::ADropDownMenuButton* UI::ADropDownMenu::AddButton(ADropDownMenuButton* Component, int Index)
{
	Area.SetHeight(Area.GetHeight() + EntryDimensions.y);

	Component->SetDimensions(EntryDimensions);
	Component->SetIconDimensions(EntryDimensions);
	Component->SetHorizontalAnchor(Area.TL.x);
	Component->Padding = FRectangle();
	Component->bSizeToText = true;

	return static_cast<ADropDownMenuButton*>(AContainer::AddComponent(Component, Index));
}

UI::ADropDownMenuCheckbox* UI::ADropDownMenu::AddCheckbox(ADropDownMenuCheckbox* Component, int Index)
{
	Area.SetHeight(Area.GetHeight() + EntryDimensions.y);

	Component->SetDimensions(EntryDimensions);
	Component->SetIconDimensions(EntryDimensions);
	Component->SetHorizontalAnchor(Area.TL.x);
	Component->Padding = FRectangle();
	Component->bSizeToText = true;

	return static_cast<ADropDownMenuCheckbox*>(AContainer::AddComponent(Component, Index));
}

bool UI::ADropDownMenu::SetOpen(bool NewbIsOpen)
{
	bIsOpen = NewbIsOpen;
	Visibility = bIsOpen ? UI_Visible : UI_Hidden;

	if (!bIsOpen)
	{
		for (UI::AComponent* Component : Components)
		{
			ParentUserInterface->ResetHoveredComponent(Component);
			Component->Visibility = Visibility;
		}
	}
	else
	{
		for each (UI::AComponent* CurrentComponent in Components)
		{
			CurrentComponent->Visibility = Visibility;
		}
	}
	return bIsOpen;
}



UI::ADropDownMenuButton::ADropDownMenuButton(AUserInterface* InParentUserInterface, FRectangle InArea, FButtonIcons InIcons) 
	: AButton(InParentUserInterface, InArea, InIcons)
{

}

UI::ADropDownMenuButton::~ADropDownMenuButton()
{
}

void UI::ADropDownMenuButton::Render(SDL_Renderer* Renderer)
{
	if (bIsHovered)
	{
		SetRenderColor(Renderer, Icons.HoveredTint);
		DrawRectangle(Renderer, DrawArea, DRAW_FILLED);
	}
	else if (bIsActivated)
	{
		SetRenderColor(Renderer, Icons.ActivatedTint);
		DrawRectangle(Renderer, DrawArea, DRAW_FILLED);
	}

	if (TextComponent)
	{
		TextComponent->Render(Renderer);
	}
}

void UI::ADropDownMenuButton::AddTextComponent(TTF_Font* InFont, wstring InText, FColor InColor)
{
	AButton::AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(0, 0)), InFont, InText, InColor, 1));
}

void UI::ADropDownMenuButton::AddTextComponent(TTF_Font* InFont, wstring* InTextPtr, FColor InColor)
{
	AButton::AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(0, 0)), InFont, InTextPtr, InColor, 1));
}

void UI::ADropDownMenuButton::OnActivated(const bool Callback)
{
	if (Callback && bCloseOnClicked)
	{
		static_cast<ADropDownMenu*>(ParentContainer)->Close();
		ParentUserInterface->ResetHoveredComponent(this);
		ParentUserInterface->ResetHoveredComponent(ParentContainer);
	}
}

void UI::ADropDownMenuButton::OnTextAdded()
{
	int Height = TTF_FontHeight(TextComponent->Font);
	TextComponent->SetAnchor(FPoint(IconArea.TL.x + Area.GetHeight() + 2, IconArea.GetCenter().y - int(Height*0.55f)));
}



UI::ADropDownMenuCheckbox::ADropDownMenuCheckbox(AUserInterface* InParentUserInterface, FRectangle InArea, FCheckboxIcons InIcons)
	: ACheckbox(InParentUserInterface, InArea, InIcons)
{
	
}

UI::ADropDownMenuCheckbox::~ADropDownMenuCheckbox()
{
}

void UI::ADropDownMenuCheckbox::Render(SDL_Renderer* Renderer)
{
	if (bIsHovered)
	{
		SetRenderColor(Renderer, Icons.HoveredTint);
		DrawRectangle(Renderer, DrawArea, DRAW_FILLED);
	}
	else if (bIsActivated)
	{
		SetRenderColor(Renderer, Icons.ActivatedTint);
		DrawRectangle(Renderer, DrawArea, DRAW_FILLED);
	}
	UpdateCheckBoxArea();

	SetRenderColor(Renderer, CheckboxIcons.BorderColor);
	DrawRectangle(Renderer, CheckBoxArea, CheckboxIcons.BorderThickness);

	if (bIsChecked)
	{
		DrawRectangle(Renderer, FRectangle(CheckBoxArea.TL + FPoint(CheckboxIcons.BorderThickness + 1), CheckBoxArea.BR - FPoint(CheckboxIcons.BorderThickness + 1)), DRAW_FILLED);
	}

	if (TextComponent)
	{
		TextComponent->Render(Renderer);
	}
}

void UI::ADropDownMenuCheckbox::AddTextComponent(TTF_Font* InFont, wstring InText, FColor InColor)
{
	AButton::AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(0, 0)), InFont, InText, InColor, 1));
}

void UI::ADropDownMenuCheckbox::AddTextComponent(TTF_Font* InFont, wstring* InTextPtr, FColor InColor)
{
	AButton::AddTextComponent(new AText(ParentUserInterface, FRectangle(GetRelativePosition(0, 0)), InFont, InTextPtr, InColor, 1));
}

void UI::ADropDownMenuCheckbox::OnActivated(const bool Callback)
{
	if (Callback && bCloseOnClicked)
	{
		static_cast<ADropDownMenu*>(ParentContainer)->Close();
		ParentUserInterface->ResetHoveredComponent(this);
		ParentUserInterface->ResetHoveredComponent(ParentContainer);
	}
	bIsChecked = !bIsChecked;
}

void UI::ADropDownMenuCheckbox::OnTextAdded()
{
	int Height = TTF_FontHeight(TextComponent->Font);
	TextComponent->SetAnchor(FPoint(IconArea.TL.x + Area.GetHeight() + 2, IconArea.GetCenter().y - int(Height*0.55f)));
}



UI::AMenuBar::AMenuBar(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, FSize InEntrySize, FColor InColor, FColor InTextColor, FColor InHoveredColor, FColor InActivatedColor, FColor InOpendedColor) 
	: AContainer(InParentUserInterface, InArea)
{
	Font = InFont;
	Color = InColor;
	TextColor = InTextColor;
	HoveredColor = InHoveredColor;
	ActivatedColor = InActivatedColor;
	OpendedColor = InOpendedColor;
	EntrySize = InEntrySize;
}

UI::AMenuBar::~AMenuBar()
{
}

void UI::AMenuBar::Tick()
{
	int MouseX = -1;
	if (Area.Contains(ParentUserInterface->ParentWindow->MousePosition) /*&& ParentUserInterface->SetHoveredComponent(this)*/)
	{
		MouseX = ParentUserInterface->ParentWindow->MousePosition.x;
	}
	else if (OpenedEntry && !OpenedEntry->GetIsOpen())
	{
		OpenedEntry->bIsHovered = false;
		OpenedEntry = nullptr;
	}
	int x = Area.TL.x;
	for each (AMenuBarEntry* Entry in Components)
	{
		Entry->Anchor = x;
		Entry->DropDownMenu->SetAnchor(FPoint(x, Area.BR.y));

		x += Entry->Size.x + Entry->Size.y;
		if (Utils::Between(MouseX, Entry->Anchor, x-1))
		{
			if (!Entry->bIsHovered && OpenedEntry)
			{
				OpenedEntry->Close();
				OpenedEntry = Entry;
				Entry->Open();
			}
			Entry->bIsHovered = true;

			if (ParentUserInterface->ParentWindow->MouseEvents.Has(Mouse_LDown))
			{
				Entry->bIsActivated = true;
			}
			else if (Entry->bIsActivated && ParentUserInterface->ParentWindow->MouseEvents.Has(Mouse_LUp))
			{
				Entry->bIsActivated = false;
				OpenedEntry = Entry;
				Entry->Open();
			}
			else
			{
				Entry->bIsActivated = false;
			}

		}
		else
		{
			Entry->bIsActivated = Entry->bIsHovered = false;

		}
	}
	AContainer::Tick();

}

void UI::AMenuBar::Render(SDL_Renderer* Renderer)
{
	SetRenderColor(Renderer, Color);
	DrawRectangle(Renderer, Area, DRAW_FILLED);
	AContainer::Render(Renderer);
}

UI::AMenuBarEntry* UI::AMenuBar::AddEntry(wstring Name)
{
	AMenuBarEntry* NewEntry = new AMenuBarEntry(ParentUserInterface, EntrySize, this, Font, Name);
	AddComponent(NewEntry);
	return NewEntry;
}

UI::AMenuBarEntry* UI::AMenuBar::AddEntry(wstring* NamePtr)
{
	AMenuBarEntry* NewEntry = new AMenuBarEntry(ParentUserInterface, EntrySize, this, Font, NamePtr);
	AddComponent(NewEntry);
	return NewEntry;
}

UI::AMenuBarEntry* UI::AMenuBar::GetEntry(wstring Name) const
{
	for each (AComponent* Entry in Components)
	{
		RETURN_IF(Name == ((UI::AMenuBarEntry*) Entry)->Name) ((AMenuBarEntry*) Entry);
	}
	return NULL;
}

UI::AMenuBarEntry* UI::AMenuBar::GetEntry(wstring* NamePtr) const
{
	for each (AComponent* Entry in Components)
	{
		RETURN_IF(NamePtr == ((UI::AMenuBarEntry*) Entry)->NamePtr) ((AMenuBarEntry*) Entry);
	}
	return NULL;
}




UI::AMenuBarEntry::AMenuBarEntry(AUserInterface* InParentUserInterface, FSize InEntrySize, AMenuBar* InMenuBar, TTF_Font* InFont, wstring InName) : AComponent(InParentUserInterface, FRectangle(FPoint(),  InEntrySize))
{
	MenuBar = InMenuBar;
	Font = InFont;
	SetName(InName);
	DropDownMenu = new ADropDownMenu(ParentUserInterface, InEntrySize, MenuBar->Color, MenuBar->OpendedColor);
	DropDownMenu->bAutoClose = false;
}

UI::AMenuBarEntry::AMenuBarEntry(AUserInterface * InParentUserInterface, FSize InEntrySize, AMenuBar* InMenuBar, TTF_Font* InFont, wstring* InNamePtr) 
	: AMenuBarEntry(InParentUserInterface, InEntrySize,InMenuBar, InFont, *InNamePtr)
{
	NamePtr = InNamePtr;
}

UI::AMenuBarEntry::~AMenuBarEntry()
{
}

void UI::AMenuBarEntry::Tick()
{
	AComponent::Tick();

	if (bIsOpended)
	{
		DropDownMenu->Tick();

		if (!bIsHovered && !DropDownMenu->GetIsHovered())
		{
			Close();
			if (MenuBar->OpenedEntry == this)
			{
				MenuBar->OpenedEntry = nullptr;
			}
			//ParentUserInterface->ResetHoveredComponent(MenuBar);
		}

	}
	if (NamePtr && *NamePtr != Name)
	{
		SetName(*NamePtr);
	}
}

void UI::AMenuBarEntry::Render(SDL_Renderer* Renderer)
{
	AComponent::Render(Renderer);

	if (bIsOpended)
	{
		

		SetRenderColor(Renderer, DropDownMenu->BackgroundColor);

		DrawRectangle(Renderer, FRectangle(Anchor, MenuBar->GetArea().TL.y, Size.x + Size.y, MenuBar->GetDimensions().y));

		SetRenderColor(Renderer, MenuBar->OpendedColor);
		DrawRectangle(Renderer, FRectangle(Anchor, MenuBar->GetArea().TL.y, Size.x + Size.y, MenuBar->GetDimensions().y), 1);

		DropDownMenu->Render(Renderer);

		SetRenderColor(Renderer, DropDownMenu->BackgroundColor);

		DrawRectangle(Renderer, FRectangle(Anchor+1, MenuBar->GetArea().BR.y-1, Size.x + Size.y-2, 2));


	}

	else if (bIsActivated)
	{
		SetRenderColor(Renderer, MenuBar->ActivatedColor);
		DrawRectangle(Renderer, FRectangle(Anchor, MenuBar->GetArea().TL.y, Size.x + Size.y, MenuBar->GetDimensions().y));
	}
	else if (bIsHovered)
	{
		SetRenderColor(Renderer, MenuBar->HoveredColor);
		DrawRectangle(Renderer, FRectangle(Anchor, MenuBar->GetArea().TL.y, Size.x + Size.y, MenuBar->GetDimensions().y));
	}

	DrawFont(Renderer, FPoint(Anchor + Size.y/2, MenuBar->GetArea().GetCenter().y - Size.y/2-1), Name, Font, MenuBar->TextColor, false, ETextDrawType::Blended);
}

void UI::AMenuBarEntry::SetName(wstring NewName)
{
	Name = NewName;
	TTF_SizeUNICODE(Font, (UShort*) Name.data(), &Size.x, &Size.y);
}




UI::APromt::APromt(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, wstring Message, wstring Buttons, FColor InColor, FColor InTextColor, FColor InHoveredColor, FColor InActivatedColor, FColor InOpendedColor)
	: AContainer(InParentUserInterface, InArea)
{
	Font = InFont;
	Color = InColor;
	TextColor = InTextColor;
	HoveredColor = InHoveredColor;
	ActivatedColor = InActivatedColor;
}

UI::APromt::~APromt()
{
}

void UI::APromt::Tick()
{
	AContainer::Tick();
}

void UI::APromt::Render(SDL_Renderer* Renderer)
{
	AContainer::Render(Renderer);
}

void UI::APromt::GetInput(FCallback Callback)
{
	boost::thread(boost::bind(&APromt::WaitForInput, this, Callback));
}

void UI::APromt::WaitForInput(FCallback Callback)
{
	while (Result.bWaiting)
	{
		THREAD_SLEEP_MS(200);
	}
	Callback.Callback();
}

void UI::APromt::ReceiveInput(AButton* Button)
{
	Result.bWaiting = false;

	for (UShort Index = 0; Index < Components.size(); Index++)
	{
		if (Components[Index] == Button)
		{
			Result.Index = Index;
			Result.ButtonText = Button->GetTextComponent()->GetText();

		}
	}
}
