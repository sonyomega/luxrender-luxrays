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

#include "bidirhybrid/bidirhybrid.h"

using namespace std;
using namespace luxrays;
using namespace luxrays::sdl;
using namespace luxrays::utils;

namespace slg {

//------------------------------------------------------------------------------
// BiDirCPURenderEngine
//------------------------------------------------------------------------------

BiDirHybridRenderEngine::BiDirHybridRenderEngine(RenderConfig *rcfg, Film *flm, boost::mutex *flmMutex) :
		HybridRenderEngine(rcfg, flm, flmMutex) {
	// For classic BiDir, the count is always 1
	eyePathCount = 1;
	lightPathCount = 1;

	film->EnableOverlappedScreenBufferUpdate(true);
}

void BiDirHybridRenderEngine::StartLockLess() {
	const Properties &cfg = renderConfig->cfg;

	//--------------------------------------------------------------------------
	// Rendering parameters
	//--------------------------------------------------------------------------

	maxEyePathDepth = cfg.GetInt("path.maxdepth", 5);
	maxLightPathDepth = cfg.GetInt("light.maxdepth", 5);
	rrDepth = cfg.GetInt("light.russianroulette.depth", cfg.GetInt("path.russianroulette.depth", 3));
	rrImportanceCap = cfg.GetFloat("light.russianroulette.cap", cfg.GetFloat("path.russianroulette.cap", .5f));

	HybridRenderEngine::StartLockLess();
}

}
