#include "pathgpu2/kernels/kernels.h"
std::string luxrays::KernelSource_PathGPU2_samplers = 
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
"void GenerateCameraPath(\n"
"		__global GPUTask *task,\n"
"		__global Ray *ray,\n"
"		Seed *seed,\n"
"		__global Camera *camera\n"
"		) {\n"
"	__global Sample *sample = &task->sample;\n"
"\n"
"	GenerateCameraRay(sample, ray\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"			, seed\n"
"#endif\n"
"			, camera);\n"
"\n"
"	sample->radiance.r = 0.f;\n"
"	sample->radiance.g = 0.f;\n"
"	sample->radiance.b = 0.f;\n"
"\n"
"	// Initialize the path state\n"
"	task->pathState.depth = 0;\n"
"	task->pathState.throughput.r = 1.f;\n"
"	task->pathState.throughput.g = 1.f;\n"
"	task->pathState.throughput.b = 1.f;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	task->pathState.bouncePdf = 1.f;\n"
"	task->pathState.specularBounce = TRUE;\n"
"#endif\n"
"	task->pathState.state = PATH_STATE_NEXT_VERTEX;\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Inlined Random Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->pixelIndex = PixelIndexInt(gid);\n"
"\n"
"	sample->u[IDX_SCREEN_X] = RndFloatValue(seed);\n"
"	sample->u[IDX_SCREEN_Y] = RndFloatValue(seed);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays,\n"
"		__global Camera *camera\n"
"		) {\n"
"	const size_t gid = get_global_id(0);\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	if (task->pathState.state == PATH_STATE_DONE) {\n"
"		__global Sample *sample = &task->sample;\n"
"\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		// Move to the next assigned pixel\n"
"		sample->pixelIndex = NextPixelIndex(sample->pixelIndex);\n"
"\n"
"		sample->u[IDX_SCREEN_X] = RndFloatValue(&seed);\n"
"		sample->u[IDX_SCREEN_Y] = RndFloatValue(&seed);\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed, camera);\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Random Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 1)\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->pixelIndex = PixelIndexInt(gid);\n"
"\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		sample->u[i] = RndFloatValue(seed);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays,\n"
"		__global Camera *camera) {\n"
"	const size_t gid = get_global_id(0);\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	if (task->pathState.state == PATH_STATE_DONE) {\n"
"		__global Sample *sample = &task->sample;\n"
"\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		// Move to the next assigned pixel\n"
"		sample->pixelIndex = NextPixelIndex(sample->pixelIndex);\n"
"\n"
"		for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"			sample->u[i] = RndFloatValue(&seed);\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed, camera);\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Metropolis Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 2)\n"
"\n"
"void LargeStep(Seed *seed, const uint largeStepCount, __global float *proposedU) {\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = RndFloatValue(seed);\n"
"}\n"
"\n"
"float Mutate(Seed *seed, const float x) {\n"
"	const float s1 = 1.f / 512.f;\n"
"	const float s2 = 1.f / 16.f;\n"
"\n"
"	const float randomValue = RndFloatValue(seed);\n"
"\n"
"	const float dx = s1 / (s1 / s2 + fabs(2.f * randomValue - 1.f)) -\n"
"		s1 / (s1 / s2 + 1.f);\n"
"\n"
"	float mutatedX = x;\n"
"	if (randomValue < 0.5f) {\n"
"		mutatedX += dx;\n"
"		mutatedX = (mutatedX < 1.f) ? mutatedX : (mutatedX - 1.f);\n"
"	} else {\n"
"		mutatedX -= dx;\n"
"		mutatedX = (mutatedX < 0.f) ? (mutatedX + 1.f) : mutatedX;\n"
"	}\n"
"\n"
"	return mutatedX;\n"
"}\n"
"\n"
"void SmallStep(Seed *seed, __global float *currentU, __global float *proposedU) {\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = Mutate(seed, currentU[i]);\n"
"}\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->totalI = 0.f;\n"
"	sample->largeMutationCount = 0.f;\n"
"\n"
"	sample->current = 0xffffffffu;\n"
"	sample->proposed = 1;\n"
"\n"
"	sample->smallMutationCount = 0;\n"
"	sample->consecutiveRejects = 0;\n"
"\n"
"	sample->weight = 0.f;\n"
"	sample->currentRadiance.r = 0.f;\n"
"	sample->currentRadiance.g = 0.f;\n"
"	sample->currentRadiance.b = 0.f;\n"
"\n"
"	LargeStep(seed, 0, &sample->u[1][0]);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays,\n"
"		__global Camera *camera) {\n"
"	const size_t gid = get_global_id(0);\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	__global Sample *sample = &task->sample;\n"
"	const uint current = sample->current;\n"
"\n"
"	// Check if it is a complete path and not the very first sample\n"
"	if ((current != 0xffffffffu) && (task->pathState.state == PATH_STATE_DONE)) {\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		const uint proposed = sample->proposed;\n"
"		__global float *proposedU = &sample->u[proposed][0];\n"
"\n"
"		if (RndFloatValue(&seed) < PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE) {\n"
"			LargeStep(&seed, sample->largeMutationCount, proposedU);\n"
"			sample->smallMutationCount = 0;\n"
"		} else {\n"
"			__global float *currentU = &sample->u[current][0];\n"
"\n"
"			SmallStep(&seed, currentU, proposedU);\n"
"			sample->smallMutationCount += 1;\n"
"		}\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed, camera);\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"void Sampler_MLT_SplatSample(__global Pixel *frameBuffer, Seed *seed, __global Sample *sample) {\n"
"	uint current = sample->current;\n"
"	uint proposed = sample->proposed;\n"
"\n"
"	Spectrum radiance = sample->radiance;\n"
"\n"
"	if (current == 0xffffffffu) {\n"
"		// It is the very first sample, I have still to initialize the current\n"
"		// sample\n"
"\n"
"		sample->currentRadiance = radiance;\n"
"		sample->totalI = Spectrum_Y(&radiance);\n"
"\n"
"		// The following 2 lines could be moved in the initialization code\n"
"		sample->largeMutationCount = 1;\n"
"		sample->weight = 0.f;\n"
"\n"
"		current = proposed;\n"
"		proposed ^= 1;\n"
"	} else {\n"
"		const Spectrum currentL = sample->currentRadiance;\n"
"		const float currentI = Spectrum_Y(&currentL);\n"
"\n"
"		const Spectrum proposedL = radiance;\n"
"		float proposedI = Spectrum_Y(&proposedL);\n"
"		proposedI = isinf(proposedI) ? 0.f : proposedI;\n"
"\n"
"		float totalI = sample->totalI;\n"
"		uint largeMutationCount = sample->largeMutationCount;\n"
"		uint smallMutationCount = sample->smallMutationCount;\n"
"		if (smallMutationCount == 0) {\n"
"			// It is a large mutation\n"
"			totalI += Spectrum_Y(&proposedL);\n"
"			largeMutationCount += 1;\n"
"\n"
"			sample->totalI = totalI;\n"
"			sample->largeMutationCount = largeMutationCount;\n"
"		}\n"
"\n"
"		const float meanI = (totalI > 0.f) ? (totalI / largeMutationCount) : 1.f;\n"
"\n"
"		// Calculate accept probability from old and new image sample\n"
"		uint consecutiveRejects = sample->consecutiveRejects;\n"
"\n"
"		float accProb;\n"
"		if ((currentI > 0.f) && (consecutiveRejects < PARAM_SAMPLER_METROPOLIS_MAX_CONSECUTIVE_REJECT))\n"
"			accProb = min(1.f, proposedI / currentI);\n"
"		else\n"
"			accProb = 1.f;\n"
"\n"
"		const float newWeight = accProb + ((smallMutationCount == 0) ? 1.f : 0.f);\n"
"		float weight = sample->weight;\n"
"		weight += 1.f - accProb;\n"
"\n"
"		const float rndVal = RndFloatValue(seed);\n"
"\n"
"		/*if (get_global_id(0) == 0)\n"
"			printf(\"[%d] Current: (%f, %f, %f) [%f] Proposed: (%f, %f, %f) [%f] accProb: %f <%f>\\n\",\n"
"					smallMutationCount,\n"
"					currentL.r, currentL.g, currentL.b, weight,\n"
"					proposedL.r, proposedL.g, proposedL.b, newWeight,\n"
"					accProb, rndVal);*/\n"
"\n"
"		Spectrum contrib;\n"
"		float norm;\n"
"		float scrX, scrY;\n"
"\n"
"		if ((accProb == 1.f) || (rndVal < accProb)) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tACCEPTED !\\n\");*/\n"
"\n"
"			// Add accumulated contribution of previous reference sample\n"
"			norm = weight / (currentI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = currentL;\n"
"\n"
"			scrX = sample->u[current][IDX_SCREEN_X];\n"
"			scrY = sample->u[current][IDX_SCREEN_Y];\n"
"\n"
"#if defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"			// Debug code: to check sample distribution\n"
"			contrib.r = contrib.g = contrib.b = (consecutiveRejects + 1.f)  * .01f;\n"
"			const uint pixelIndex = PPixelIndexFloat2D(scrX, scrY);\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, 1.f);\n"
"#endif\n"
"\n"
"			current ^= 1;\n"
"			proposed ^= 1;\n"
"			consecutiveRejects = 0;\n"
"\n"
"			weight = newWeight;\n"
"\n"
"			sample->currentRadiance = proposedL;\n"
"		} else {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tREJECTED !\\n\");*/\n"
"\n"
"			// Add contribution of new sample before rejecting it\n"
"			norm = newWeight / (proposedI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = proposedL;\n"
"\n"
"			scrX = sample->u[proposed][IDX_SCREEN_X];\n"
"			scrY = sample->u[proposed][IDX_SCREEN_Y];\n"
"\n"
"			++consecutiveRejects;\n"
"\n"
"#if defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"			// Debug code: to check sample distribution\n"
"			contrib.r = contrib.g = contrib.b = 1.f * .01f;\n"
"			const uint pixelIndex = PixelIndexFloat2D(scrX, scrY);\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, 1.f);\n"
"#endif\n"
"		}\n"
"\n"
"#if !defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"		if (norm > 0.f) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tPixelIndex: %d Contrib: (%f, %f, %f) [%f] consecutiveRejects: %d\\n\",\n"
"						pixelIndex, contrib.r, contrib.g, contrib.b, norm, consecutiveRejects);*/\n"
"\n"
"#if (PARAM_IMAGE_FILTER_TYPE == 0)\n"
"			const uint pixelIndex = PixelIndexFloat2D(scrX, scrY);\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, norm);\n"
"#else\n"
"			float sx, sy;\n"
"			const uint pixelIndex = PixelIndexFloat2DWithOffset(scrX, scrY, &sx, &sy);\n"
"			SplatSample(frameBuffer, pixelIndex, sx, sy, &contrib, norm);\n"
"#endif\n"
"		}\n"
"#endif\n"
"\n"
"		sample->weight = weight;\n"
"		sample->consecutiveRejects = consecutiveRejects;\n"
"	}\n"
"\n"
"	sample->current = current;\n"
"	sample->proposed = proposed;\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Stratified Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 3)\n"
"\n"
"void StratifiedSample1D(__local float *buff, Seed *seed) {\n"
"	const float dx = 1.f / PARAM_SAMPLER_STRATIFIED_X_SAMPLES;\n"
"\n"
"	for (uint x = 0; x < PARAM_SAMPLER_STRATIFIED_X_SAMPLES; ++x) {\n"
"		*buff++ = (x + RndFloatValue(seed)) * dx;\n"
"	}\n"
"}\n"
"\n"
"void Shuffle1D(__local float *buff, Seed *seed) {\n"
"	const uint count = PARAM_SAMPLER_STRATIFIED_X_SAMPLES;\n"
"\n"
"	for (uint i = 0; i < count; ++i) {\n"
"		const uint other = RndUintValue(seed) % (count - i);\n"
"\n"
"		const float u0 = buff[other];\n"
"		buff[other] = buff[i];\n"
"		buff[i] = u0;\n"
"	}\n"
"}\n"
"\n"
"void StratifiedSample2D(__local float *buff, Seed *seed) {\n"
"	const float dx = 1.f / PARAM_SAMPLER_STRATIFIED_X_SAMPLES;\n"
"	const float dy = 1.f / PARAM_SAMPLER_STRATIFIED_Y_SAMPLES;\n"
"\n"
"	for (uint y = 0; y < PARAM_SAMPLER_STRATIFIED_Y_SAMPLES; ++y) {\n"
"		for (uint x = 0; x < PARAM_SAMPLER_STRATIFIED_X_SAMPLES; ++x) {\n"
"			*buff++ = (x + RndFloatValue(seed)) * dx;\n"
"			*buff++ = (y + RndFloatValue(seed)) * dy;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"void Shuffle2D(__local float *buff, Seed *seed) {\n"
"	const uint count = PARAM_SAMPLER_STRATIFIED_X_SAMPLES *  PARAM_SAMPLER_STRATIFIED_Y_SAMPLES;\n"
"\n"
"	for (uint i = 0; i < count; ++i) {\n"
"		const uint other = RndUintValue(seed) % (count - i);\n"
"\n"
"		uint otherIdx = 2 * other;\n"
"		uint iIdx = 2 * i;\n"
"\n"
"		const float u0 = buff[otherIdx];\n"
"		buff[otherIdx] = buff[iIdx];\n"
"		buff[iIdx] = u0;\n"
"\n"
"		++otherIdx;\n"
"		++iIdx;\n"
"		const float u1 = buff[otherIdx];\n"
"		buff[otherIdx] = buff[iIdx];\n"
"		buff[iIdx] = u1;\n"
"	}\n"
"}\n"
"\n"
"void Copy2D(__local float *src, __global float *dest) {\n"
"	for (uint i = 0; i < PARAM_SAMPLER_STRATIFIED_X_SAMPLES *  PARAM_SAMPLER_STRATIFIED_Y_SAMPLES; ++i) {\n"
"		*dest++ = *src++;\n"
"		*dest++ = *src++;\n"
"	}\n"
"}\n"
"\n"
"void Copy1D(__local float *src, __global float *dest) {\n"
"	for (uint i = 0; i < PARAM_SAMPLER_STRATIFIED_X_SAMPLES *  PARAM_SAMPLER_STRATIFIED_Y_SAMPLES; ++i)\n"
"		*dest++ = *src++;\n"
"}\n"
"\n"
"void Sampler_StratifiedBufferInit(__local float *localMemTempBuff,\n"
"		Seed *seed, __global Sample *sample) {\n"
"	__local float *tempBuff = &localMemTempBuff[get_local_id(0) * PARAM_SAMPLER_STRATIFIED_X_SAMPLES * PARAM_SAMPLER_STRATIFIED_Y_SAMPLES * 2];\n"
"\n"
"	StratifiedSample2D(tempBuff, seed);\n"
"	Copy2D(tempBuff, &sample->stratifiedScreen2D[0]);\n"
"\n"
"#if defined(PARAM_CAMERA_HAS_DOF)\n"
"	StratifiedSample2D(tempBuff, seed);\n"
"	Shuffle2D(tempBuff, seed);\n"
"	Copy2D(tempBuff, &sample->stratifiedDof2D[0]);\n"
"#endif\n"
"\n"
"#if defined(PARAM_HAS_ALPHA_TEXTUREMAPS)\n"
"	StratifiedSample1D(tempBuff, seed);\n"
"	Shuffle1D(tempBuff, seed);\n"
"	Copy1D(tempBuff, &sample->stratifiedAlpha1D[0]);\n"
"#endif\n"
"\n"
"	StratifiedSample2D(tempBuff, seed);\n"
"	Shuffle2D(tempBuff, seed);\n"
"	Copy2D(tempBuff, &sample->stratifiedBSDF2D[0]);\n"
"\n"
"	StratifiedSample1D(tempBuff, seed);\n"
"	Shuffle1D(tempBuff, seed);\n"
"	Copy1D(tempBuff, &sample->stratifiedBSDF1D[0]);\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	StratifiedSample2D(tempBuff, seed);\n"
"	Shuffle2D(tempBuff, seed);\n"
"	Copy2D(tempBuff, &sample->stratifiedLight2D[0]);\n"
"\n"
"	StratifiedSample1D(tempBuff, seed);\n"
"	Shuffle1D(tempBuff, seed);\n"
"	Copy1D(tempBuff, &sample->stratifiedLight1D[0]);\n"
"#endif\n"
"}\n"
"\n"
"void Sampler_CopyFromStratifiedBuffer(Seed *seed, __global Sample *sample, const uint index) {\n"
"	const uint i0 = index * 2;\n"
"	const uint i1 = i0 + 1;\n"
"\n"
"	sample->u[IDX_SCREEN_X] = sample->stratifiedScreen2D[i0];\n"
"	sample->u[IDX_SCREEN_Y] = sample->stratifiedScreen2D[i1];\n"
"\n"
"#if defined(PARAM_CAMERA_HAS_DOF)\n"
"	sample->u[IDX_DOF_X] = sample->stratifiedDof2D[i0];\n"
"	sample->u[IDX_DOF_Y] = sample->stratifiedDof2D[i1];\n"
"#endif\n"
"\n"
"#if defined(PARAM_HAS_ALPHA_TEXTUREMAPS)\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_TEX_ALPHA] = sample->stratifiedAlpha1D[index];\n"
"#endif\n"
"\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_BSDF_X] = sample->stratifiedBSDF2D[i0];\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_BSDF_Y] = sample->stratifiedBSDF2D[i1];\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_BSDF_Z] = sample->stratifiedBSDF1D[index];\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_DIRECTLIGHT_X] = sample->stratifiedLight2D[i0];\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_DIRECTLIGHT_Y] = sample->stratifiedLight2D[i1];\n"
"	sample->u[IDX_BSDF_OFFSET + IDX_DIRECTLIGHT_Z] = sample->stratifiedLight1D[index];\n"
"#endif\n"
"\n"
"	for (int i = IDX_BSDF_OFFSET + IDX_RR; i < TOTAL_U_SIZE; ++i)\n"
"		sample->u[i] = RndFloatValue(seed);\n"
"}\n"
"\n"
"void Sampler_Init(const size_t gid, __local float *localMemTempBuff,\n"
"		Seed *seed, __global Sample *sample) {\n"
"	sample->pixelIndex = PixelIndexInt(gid);\n"
"\n"
"	Sampler_StratifiedBufferInit(localMemTempBuff, seed, sample);\n"
"\n"
"	Sampler_CopyFromStratifiedBuffer(seed, sample, 0);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays,\n"
"		__global Camera *camera,\n"
"		__local float *localMemTempBuff\n"
"		) {\n"
"	const size_t gid = get_global_id(0);\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	if (task->pathState.state == PATH_STATE_DONE) {\n"
"		__global Sample *sample = &task->sample;\n"
"\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		// Check if I have used all the stratified samples\n"
"		const uint sampleNewCount = taskStats[gid].sampleCount + 1;\n"
"		const uint sampleNewIndex = sampleNewCount % (PARAM_SAMPLER_STRATIFIED_X_SAMPLES * PARAM_SAMPLER_STRATIFIED_Y_SAMPLES);\n"
"\n"
"		if (sampleNewIndex == 0) {\n"
"			// Move to the next assigned pixel\n"
"			sample->pixelIndex = NextPixelIndex(sample->pixelIndex);\n"
"\n"
"			// Initialize the stratified buffer\n"
"			Sampler_StratifiedBufferInit(localMemTempBuff, &seed, sample);\n"
"		}\n"
"\n"
"		Sampler_CopyFromStratifiedBuffer(&seed, sample, sampleNewIndex);\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed, camera);\n"
"\n"
"		taskStats[gid].sampleCount = sampleNewCount;\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
;
