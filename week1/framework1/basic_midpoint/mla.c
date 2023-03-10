/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
 * Description ..... Midpoint Line Algorithm
 * Created by ...... Jurgen Sturm
 *
 * Student 1 name: Jan Deen
 * Student 1 id  : 12873551
 * Student 2 name: Gilian Honkoop
 * Student 2 id  : 13710729
 * Date          : 9-9-2022
 * Comments      : Mirroring was used to use a solution for 1 octant
 *                 in all 8 octants.
 *
 * (always fill in these fields before submitting!!)
 */

#include "SDL.h"
#include "init.h"
#include <stdio.h>

/*
 * Midpoint Line Algorithm
 *
 * As you probably will have figured out, this is the part where you prove
 * your programming skills. The code in the mla function should draw a direct
 * line between (x0,y0) and (x1,y1) in the specified color.
 *
 * Until now, the example code below draws only a horizontal line between
 * (x0,y0) and (x1,y0) and a vertical line between (x1,y1).
 *
 * And here the challenge begins..
 *
 * Good luck!
 *
 *
 */


void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  PutPixel(t,x0,y0,colour);
  PutPixel(t,x1,y1,colour);

  int mult_y = 1;
  int switched = 0;

  // Check whether the coordinates must be switched.
  if (x1 - x0 < 0) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
    temp = y0;
    y0 = y1;
    y1 = temp;
  }

  // Check whether the coordinates must be mirrored in the x-axis.
  if (y1 - y0 < 0 ) {
    y0 = -y0;
    y1 = -y1;
    mult_y = -1;
  }

  // Check whether the x- amd y-axis must be switched.
  if ((x1 - x0) - (y1 - y0) < 0) {
    int temp = x0;
    x0 = y0;
    y0 = temp;
    temp = x1;
    x1 = y1;
    y1 = temp;
    switched = 1;
  }

  int x;
  float m = (float)(y1 - y0) / (x1 -x0);
  float last_y = y0;
  int y_level = y0;


  for (x = x0 + 1; x != x1; x++) {
    float new_y = last_y + m;

    // Check if the new y-value is above the midpoint.
    if (new_y > y_level + 0.5) {
      // Check whether the x and y were switched.
      // The multiplier must also be switched if the coordinates
      // were mirrored in the x-axis.
      if (switched) {
        PutPixel(t, (y_level + 1), x * mult_y, colour);
      }
      else {
        PutPixel(t, x, (y_level + 1) * mult_y, colour);
      }

      y_level++;
    }
    else {
      // Check whether the x and y were switched.
      // The multiplier must also be switched if the coordinates
      // were mirrored in the x-axis.
      if (switched) {
        PutPixel(t, y_level, x * mult_y, colour);
      }
      else {
        PutPixel(t, x, y_level * mult_y, colour);
      }

    }
      last_y = new_y;
  }

  return;
}
