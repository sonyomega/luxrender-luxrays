#line 2 "randomgen_funcs.cl"

/***************************************************************************
 *   Copyright (C) 1998-2010 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of LuxRays.                                         *
 *                                                                         *
 *   LuxRays is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   LuxRays is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   LuxRays website: http://www.luxrender.net                             *
 ***************************************************************************/

void ConcentricSampleDisk(const float u1, const float u2, float *dx, float *dy) {
	float r, theta;
	// Map uniform random numbers to $[-1,1]^2$
	float sx = 2.f * u1 - 1.f;
	float sy = 2.f * u2 - 1.f;
	// Map square to $(r,\theta)$
	// Handle degeneracy at the origin
	if (sx == 0.f && sy == 0.f) {
		*dx = 0.f;
		*dy = 0.f;
		return;
	}
	if (sx >= -sy) {
		if (sx > sy) {
			// Handle first region of disk
			r = sx;
			if (sy > 0.f)
				theta = sy / r;
			else
				theta = 8.f + sy / r;
		} else {
			// Handle second region of disk
			r = sy;
			theta = 2.f - sx / r;
		}
	} else {
		if (sx <= sy) {
			// Handle third region of disk
			r = -sx;
			theta = 4.f - sy / r;
		} else {
			// Handle fourth region of disk
			r = -sy;
			theta = 6.f + sx / r;
		}
	}
	theta *= M_PI / 4.f;
	*dx = r * cos(theta);
	*dy = r * sin(theta);
}
