#include "stdafx.h"
#include "Debugs.h"
#include "DrawHelpers.h"
#include "AApp.h"
#include "AUserInterface.h"

#define MESH_SIZE 20

MESH_DEBUG(
ADebugAlkaneMesh::ADebugAlkaneMesh()
{
}

ADebugAlkaneMesh::~ADebugAlkaneMesh()
{
}

void ADebugAlkaneMesh::Render()
{
	Window = &Globals::App->Parser.DebugWindow;

	SetRenderColor(Window->Renderer, FColor(128));
	SDL_RenderClear(Window->Renderer);
	for each (AlkaneMesh::AVertex* Vertex in Mesh.GetVertices())
	{
		if (Vertex->SingleCycloBridge)
		{

			SetRenderColor(Window->Renderer, FColor(255, 255, 0, 128));

			//DrawRectangle(Window->Renderer, FRectangle(Vertex->FunctionalGroup->Position * MESH_SIZE - FPoint(MESH_SIZE / 4), Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(MESH_SIZE / 4)));
			DrawRectangle(Window->Renderer, FRectangle(Vertex->FunctionalGroup->Position * MESH_SIZE - FPoint(MESH_SIZE / 4), Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(MESH_SIZE / 4)));


		}
		else
		{
			SetRenderColor(Window->Renderer, FColor(255,0,0,128));

			DrawRectangle(Window->Renderer, FRectangle(Vertex->FunctionalGroup->Position * MESH_SIZE - FPoint(MESH_SIZE / 4), Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(MESH_SIZE / 4)));

			SetRenderColor(Window->Renderer, FColor(0, 0, 255));

			//DrawFont(Window->Renderer, Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(0, -9), to_wstring(Vertex->Key), DefaultFont, FColor(0, 0, 255));
			for each (AlkaneMesh::ABridge* Bridge in Vertex->Bridges)
			{
				SetRenderColor(Window->Renderer, FColor(0, 255, 0));

				FPoint P1 = Vertex->FunctionalGroup->Position * MESH_SIZE;
				FPoint P2 = Vertex->GetBridgeEnd(Bridge)->FunctionalGroup->Position * MESH_SIZE;

				DrawLine(Window->Renderer, P1, P2);
				SetRenderColor(Window->Renderer, FColor(0, 0, 255));

				//DrawFont(Window->Renderer, P1*0.5f + P2*0.5f + FPoint(0, -9), to_wstring(Bridge->Key), DefaultFont, FColor(0, 0, 255));

			}
		}
		
	}
	for each (AlkaneMesh::FNode BridgeToVertex in Loop)
	{
		AlkaneMesh::AVertex* Vertex = BridgeToVertex.Vertex;
		AlkaneMesh::ABridge* Bridge = BridgeToVertex.Bridge;
		SetRenderColor(Window->Renderer, FColor(255, 255, 255, 128));

		DrawRectangle(Window->Renderer, FRectangle(Vertex->FunctionalGroup->Position * MESH_SIZE - FPoint(MESH_SIZE / 4), Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(MESH_SIZE / 4)));
		if (!Bridge)
		{
			continue;
		}
		SetRenderColor(Window->Renderer, FColor(255, 255, 255));

		FPoint P1 = Vertex->FunctionalGroup->Position * MESH_SIZE;
		FPoint P2 = Vertex->GetBridgeEnd(Bridge)->FunctionalGroup->Position * MESH_SIZE;

		DrawLine(Window->Renderer, P1, P2);

		//DrawFont(Window->Renderer, P1*0.5f + P2*0.5f + FPoint(0, -9), to_wstring(Bridge->GetNodeSize()), DefaultFont, FColor(0, 0, 255));


		SetRenderColor(Window->Renderer, FColor(0, 0, 255));

		//DrawFont(Window->Renderer, P1*0.5f + P2*0.5f + FPoint(0, -9), to_wstring(Bridge->Key), DefaultFont, FColor(0, 0, 255));
		//DrawFont(Window->Renderer, Vertex->FunctionalGroup->Position * MESH_SIZE + FPoint(0, -9), to_wstring(Vertex->Key), DefaultFont, FColor(0, 0, 255));


	}
	SDL_RenderPresent(Window->Renderer);
}

)