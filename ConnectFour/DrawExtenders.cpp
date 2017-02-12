#include "stdafx.h"
#include "DrawExtenders.h"

#define MAX_POLY_CORNERS 10


using namespace std;


void FillPoly(SDL_Renderer* Renderer, vector<Point> Vertecies, Rect Area)
{
	int  nodes, nodeX[MAX_POLY_CORNERS], i, j, swap;
	Point Pixel;


	for (Pixel.y = Area.TL.y; Pixel.y<Area.BR.y; Pixel.y++)
	{

		//  Build a list of nodes.
		nodes = 0; j = (int)Vertecies.size() - 1;
		for (i = 0; i<Vertecies.size(); i++)
		{
			if (Vertecies[i].y<(double)Pixel.y && Vertecies[j].y>= (double)Pixel.y
				|| Vertecies[j].y<(double)Pixel.y && Vertecies[i].y >= (double)Pixel.y)
			{
				nodeX[nodes++] = (int)(Vertecies[i].x + (Pixel.y - Vertecies[i].y) / (Vertecies[j].y - Vertecies[i].y)
					*(Vertecies[j].x - Vertecies[i].x));
			}
			j = i;
		}

		//  Sort the nodes, via a simple “Bubble” sort.
		i = 0;
		while (i<nodes - 1)
		{
			if (nodeX[i]>nodeX[i + 1])
			{
				swap = nodeX[i]; nodeX[i] = nodeX[i + 1]; nodeX[i + 1] = swap; if (i) i--;
			}
			else
			{
				i++;
			}
		}

		//  Fill the pixels between node pairs.
		for (i = 0; i<nodes; i += 2)
		{
			if (nodeX[i] >= Area.BR.x) break;
			if (nodeX[i + 1]> Area.TL.x)
			{
				if (nodeX[i]< Area.TL.x) nodeX[i] = Area.TL.x;
				if (nodeX[i + 1]> Area.BR.x) nodeX[i + 1] = Area.BR.x;
				for (Pixel.x = nodeX[i]; Pixel.x<nodeX[i + 1]; Pixel.x++) SDL_RenderDrawPoint(Renderer, Pixel.x, Pixel.y);
			}
		}
	}
}
