#ifndef AUDIOEFFECT_H
#define AUDIOEFFECT_H

#include <complex>
#include "fourier.h"

#define F_BUFF_SIZ 1024 //El tamaño del buffer para aplicar la FFT.

class AudioEffects {
public:
	virtual void effect(float *samples, unsigned int N_samples) = 0;
};

class Robotization : public AudioEffects {
	Fourier F;
	std::complex<float> *data;
	std::complex<float> buffer[F_BUFF_SIZ];
public:
	void effect(float *samples, unsigned int N_samples);
};

#endif //AUDIOEFFECT_H