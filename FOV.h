#ifndef FOV_H_INCLUDED
#define FOV_H_INCLUDED

#include "global.h"

//I didn't write this, but it used to be in C# so I kinda wrote it.

/// Implementation of "FOV using recursive shadowcasting - improved" as
/// described on http://roguebasin.roguelikedevelopment.org/index.php?title=FOV_using_recursive_shadowcasting_-_improved
///
/// The FOV code is contained in the region "FOV Algorithm".
/// The method GetVisibleCells() is called to calculate the cells
/// visible to the player by examing each octant sequantially.
/// The generic list VisiblePoints contains the cells visible to the player.
///
/// GetVisibleCells() is called everytime the player moves, and the event playerMoved
/// is called when a successful move is made (the player moves into an empty cell)

/*
	  Octant data

	    \ 1 | 2 /
	   8 \  |  / 3
	   -----+-----
	   7 /  |  \ 4
	    / 6 | 5 \

	  1 = NNW, 2 =NNE, 3=ENE, 4=ESE, 5=SSE, 6=SSW, 7=WSW, 8 = WNW
*/

class FOVRecurse
{
private:
  const std::vector<std::vector<int>>& tileset;

  const std::vector<int> VisibleOctants{1,2,3,4,5,6,7,8};

  std::vector<std::pair<int,int>>& VisiblePoints;

	/// Check if the provided coordinate is within the bounds of the mapp array
	inline bool Point_Valid(const int& pX, const int& pY)
	{
		return pX >= 0 && pX < MAP_WIDTH
					 && pY >= 0 && pY < MAP_HEIGHT;
	}

	/// Get the gradient of the slope formed by the two points
	/// pInvert Invert slope
	double GetSlope(const double& pX1, const double& pY1, const double& pX2, const double& pY2, const bool& pInvert)
	{
		if(pInvert)
			return (pY1 - pY2) / (pX1 - pX2);
		else
			return (pX1 - pX2) / (pY1 - pY2);
	}


	/// Calculate the distance between the two points
	int GetVisDistance(const int& pX1, const int& pY1, const int& pX2, const int& pY2)
	{
	  return (pX1 - pX2) + (pY1 - pY2); //square view
    //return ((pX1 - pX2) * (pX1 - pX2)) + ((pY1 - pY2) * (pY1 - pY2)); //circle view
	}
public:

  const int VisualRange = 9;
  int &player_x; int &player_y;

  FOVRecurse(const std::vector<std::vector<int>>& tileset_, std::vector<std::pair<int,int>>& view_field, int& x, int& y)
  : tileset(tileset_), VisiblePoints(view_field), player_x(x), player_y(y){
    VisiblePoints.reserve((VisualRange*VisualRange)*2); //might as well...
  }

  /// Start here: go through all the octants which surround the player to
	/// determine which open cells are visible
  void GetVisibleCells()
	{
		VisiblePoints.clear();
		for(auto o: VisibleOctants)
      ScanOctant(1, o, 1.0, 0.0);
    //I don't know why the player position is not shown, probably a rougelike thing.
    VisiblePoints.emplace_back(std::pair<int,int>(player_x,player_y));
	}
protected:
  /// Examine the provided octant and calculate the visible cells within it.
	/// pDepth: Depth of the scan
	/// pOctant: Octant being examined
	/// pStartSlope: Start slope of the octant
	/// pEndSlope: End slope of the octance
	void ScanOctant(const int& pDepth, const int& pOctant, double pStartSlope, const double& pEndSlope)
	{

		int visrange2 = VisualRange*VisualRange;
		int x = 0;
		int y = 0;

		switch(pOctant)
		{

		case 1: //nnw
			y = player_y - pDepth;
			if(y < 0) return;

			x = player_x - static_cast<int>((pStartSlope * static_cast<double>(pDepth))); //I dunno too, but in the c# version it converts it here, so I guess...
			if(x < 0) x = 0;

			while(GetSlope(x, y, player_x, player_y, false) >= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)  //current cell blocked
					{
						if(x - 1 >= 0 && tileset[x - 1][y] == tile_non_solid)  //prior cell within range AND open...
							//...incremenet the depth, adjust the endslope and recurse
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x - 0.5, y + 0.5, player_x, player_y, false));
					}
					else
					{
						if(x - 1 >= 0 && tileset[x - 1][y] == tile_solid)  //prior cell within range AND open...
							//..adjust the startslope
							pStartSlope = GetSlope(x - 0.5, y - 0.5, player_x, player_y, false);
					}
				}
				x++;
			}
			x--;
			break;

		case 2: //nne

			y = player_y - pDepth;
			if(y < 0) return;

			x = player_x + static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(x >= MAP_WIDTH) x = MAP_WIDTH - 1;

			while(GetSlope(x, y, player_x, player_y, false) <= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(x + 1 < MAP_WIDTH && tileset[x + 1][y] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x + 0.5, y + 0.5, player_x, player_y, false));
					}
					else
					{
						if(x + 1 <MAP_WIDTH && tileset[x + 1][y] == tile_solid)
							pStartSlope = -GetSlope(x + 0.5, y - 0.5, player_x, player_y, false);
					}
				}
				x--;
			}
			x++;
			break;

		case 3:

			x = player_x + pDepth;
			if(x >= MAP_WIDTH) return;

			y = player_y - static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(y < 0) y = 0;

			while(GetSlope(x, y, player_x, player_y, true) <= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(y - 1 >= 0 && tileset[x][y - 1] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x - 0.5, y - 0.5, player_x, player_y, true));
					}
					else
					{
						if(y - 1 >= 0 && tileset[x][y - 1] == tile_solid)
							pStartSlope = -GetSlope(x + 0.5, y - 0.5, player_x, player_y, true);
					}
				}
				y++;
			}
			y--;
			break;

		case 4:

			x = player_x + pDepth;
			if(x >= MAP_WIDTH) return;

			y = player_y + static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(y >= MAP_HEIGHT) y = MAP_HEIGHT - 1;

			while(GetSlope(x, y, player_x, player_y, true) >= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(y + 1 < MAP_HEIGHT && tileset[x][y + 1] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x - 0.5, y + 0.5, player_x, player_y, true));
					}
					else
					{
						if(y + 1 < MAP_HEIGHT && tileset[x][y + 1] == tile_solid)
							pStartSlope = GetSlope(x + 0.5, y + 0.5, player_x, player_y, true);
					}
				}
				y--;
			}
			y++;
			break;

		case 5:

			y = player_y + pDepth;
			if(y >= MAP_HEIGHT) return;

			x = player_x + static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(x >= MAP_WIDTH) x = MAP_WIDTH - 1;

			while(GetSlope(x, y, player_x, player_y, false) >= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(x + 1 < MAP_HEIGHT && tileset[x+1][y] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x + 0.5, y - 0.5, player_x, player_y, false));
					}
					else
					{
						if(x + 1 < MAP_HEIGHT
								&& tileset[x + 1][y] == tile_solid)
							pStartSlope = GetSlope(x + 0.5, y + 0.5, player_x, player_y, false);
					}
				}
				x--;
			}
			x++;
			break;

		case 6:

			y = player_y + pDepth;
			if(y >= MAP_HEIGHT) return;

			x = player_x - static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(x < 0) x = 0;

			while(GetSlope(x, y, player_x, player_y, false) <= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(x - 1 >= 0 && tileset[x - 1][y] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x - 0.5, y - 0.5, player_x, player_y, false));
					}
					else
					{
						if(x - 1 >= 0
								&& tileset[x - 1][y] == tile_solid)
							pStartSlope = -GetSlope(x - 0.5, y + 0.5, player_x, player_y, false);
					}
				}
				x++;
			}
			x--;
			break;

		case 7:

			x = player_x - pDepth;
			if(x < 0) return;

			y = player_y + static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(y >= MAP_HEIGHT) y = MAP_HEIGHT - 1;

			while(GetSlope(x, y, player_x, player_y, true) <= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(y + 1 < MAP_HEIGHT && tileset[x][y+1] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x + 0.5, y + 0.5, player_x, player_y, true));
					}
					else
					{
						if(y + 1 < MAP_HEIGHT && tileset[x][y + 1] == tile_solid)
							pStartSlope = -GetSlope(x - 0.5, y + 0.5, player_x, player_y, true);
					}
				}
				y--;
			}
			y++;
			break;

		case 8: //wnw

			x = player_x - pDepth;
			if(x < 0) return;

			y = player_y - static_cast<int>((pStartSlope * static_cast<double>(pDepth)));
			if(y < 0) y = 0;

			while(GetSlope(x, y, player_x, player_y, true) >= pEndSlope)
			{
				if(GetVisDistance(x, y, player_x, player_y) <= visrange2)
				{
				  VisiblePoints.emplace_back(std::pair<int,int>(x, y));
					if(tileset[x][y] == tile_solid)
					{
						if(y - 1 >=0 && tileset[x][y - 1] == tile_non_solid)
							ScanOctant(pDepth + 1, pOctant, pStartSlope, GetSlope(x + 0.5, y - 0.5, player_x, player_y, true));
					}
					else
					{
						if(y - 1 >= 0 && tileset[x][y - 1] == tile_solid)
							pStartSlope = GetSlope(x - 0.5, y - 0.5, player_x, player_y, true);
					}
				}
				y++;
			}
			y--;
			break;
		}


		if(x < 0)
			x = 0;
		else
			if(x >= MAP_WIDTH)
				x = MAP_WIDTH - 1;

		if(y < 0)
			y = 0;
		else
			if(y >= MAP_HEIGHT)
				y = MAP_HEIGHT - 1;

		if(pDepth < VisualRange && tileset[x][y] == tile_non_solid)
			ScanOctant(pDepth + 1, pOctant, pStartSlope, pEndSlope);

	}
};

#endif // FOV_H_INCLUDED
