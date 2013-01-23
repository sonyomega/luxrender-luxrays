#include <string>
namespace luxrays { namespace ocl {
std::string KernelSource_sampler_types = 
"#line 2 \"sampler_types.cl\"\n"
"\n"
"/***************************************************************************\n"
" *   Copyright (C) 1998-2010 by authors (see AUTHORS.txt )                 *\n"
" *                                                                         *\n"
" *   This file is part of LuxRays.                                         *\n"
" *                                                                         *\n"
" *   LuxRays is free software; you can redistribute it and/or modify       *\n"
" *   it under the terms of the GNU General Public License as published by  *\n"
" *   the Free Software Foundation; either version 3 of the License, or     *\n"
" *   (at your option) any later version.                                   *\n"
" *                                                                         *\n"
" *   LuxRays is distributed in the hope that it will be useful,            *\n"
" *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n"
" *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n"
" *   GNU General Public License for more details.                          *\n"
" *                                                                         *\n"
" *   You should have received a copy of the GNU General Public License     *\n"
" *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *\n"
" *                                                                         *\n"
" *   LuxRays website: http://www.luxrender.net                             *\n"
" ***************************************************************************/\n"
"\n"
"typedef enum {\n"
"	RANDOM = 0,\n"
"	METROPOLIS = 1,\n"
"	SOBOL = 2\n"
"} SamplerType;\n"
"\n"
"typedef struct {\n"
"	SamplerType type;\n"
"	union {\n"
"		struct {\n"
"			float largeMutationProbability, imageMutationRange;\n"
"			unsigned int maxRejects;\n"
"		} metropolis;\n"
"	};\n"
"} Sampler;\n"
; } }
