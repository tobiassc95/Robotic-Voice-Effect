#include <iostream>
#include "portaudio.h"
#include "audiofile.h"
#include "callback.h"
#include "audioeffect.h"

#define PA_BUFF_SIZ 512 //el tamaño del buffer de portaudio. Contendra los frames(samples)
#define SAMP_FREQ 44100 //frecuencia de sampleo. Es 44.1 KHz porque es la frecuencia standard de sampleo para audio.
#define SECONDS 5 //los segundos para grabar y reproducir.

int main(void) {
	PaStreamParameters inp_param, outp_param;
	PaStream* stream;
	PaError PA_err;
	AudioFile<float> audiof;
	DATA data;
	Robotization robotization;
	std::string option;

	//inicializamos los datos.
	data.create_array(SECONDS*SAMP_FREQ);
	audiof.setAudioBufferSize(1, SECONDS*SAMP_FREQ);
	audiof.setSampleRate(SAMP_FREQ);
	audiof.setBitDepth(16);
	PA_err = Pa_Initialize(); //inicializamos portaudio.
	if (PA_err != paNoError) {
		std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
		Pa_Terminate();
		return -1;
	}
	while (true) {
		data.item = 0; //nos posicionamos en el primer elemento del arreglo que contendra todas las muestras (frames).
		inp_param.device = Pa_GetDefaultInputDevice(); //default input device
		if (inp_param.device == paNoDevice) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		inp_param.channelCount = 1;
		inp_param.sampleFormat = paFloat32;
		inp_param.suggestedLatency = Pa_GetDeviceInfo(inp_param.device)->defaultLowInputLatency;
		inp_param.hostApiSpecificStreamInfo = NULL;

		//pregunta al usuario que efecto quiere.
		while (true) {
			std::cout << "WHICH EFFECT DO YOU WANT?: ";
			std::cin >> option;
			if (option == "NONE" || option == "none" || option == "ROBOTIZATION" || option == "robotization")
				break;
			else
				std::cout << "WHAT?" << std::endl;
		}

		//todo listo para empezar a grabar.
		PA_err = Pa_OpenStream(&stream, &inp_param, NULL, SAMP_FREQ, PA_BUFF_SIZ, paClipOff, record_cb, &data);
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		PA_err = Pa_StartStream(stream); //aca el progrma empieza a escuchar o reproducir.
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}

		//grabando...
		std::cout << "RECORDING!! (" << SECONDS << " seconds)" << std::endl;
		while (Pa_IsStreamActive(stream) == 1) { //el loop termina cuando el arreglo de muestras (samples) tiene todos los datos de la entrada.
			Pa_Sleep(100);
		}
		std::cout << "RECORDING FINISHED!! " << std::endl;
		PA_err = Pa_CloseStream(stream);
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		//Termino de grabar.

		//guardamos en un archivo wav el audio original.
		for (unsigned int i = 0; i < audiof.getNumSamplesPerChannel(); i++) {
			audiof.samples[0][i] = data.samples[i];
		}
		audiof.save("audio1.wav");

		///////////////////////////////////

		if (option == "ROBOTIZATION" || option == "robotization")
			robotization.effect(data.samples, data.N_samples);

		///////////////////////////////////
		
		//guardamos en un archivo wav el audio con el efecto aplicado.
		for (unsigned int i = 0; i < audiof.getNumSamplesPerChannel(); i++) {
			audiof.samples[0][i] = data.samples[i];
		}
		audiof.save("audio2.wav");

		//Reiniciamos parametros para reproducir lo que se grabo.
		data.item = 0; //nos posicionamos en el primer elemento del arreglo que contendra todas las muestras (frames).
		outp_param.device = Pa_GetDefaultOutputDevice(); //default output device.
		if (outp_param.device == paNoDevice) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		outp_param.channelCount = 1;
		outp_param.sampleFormat = paFloat32;
		outp_param.suggestedLatency = Pa_GetDeviceInfo(outp_param.device)->defaultLowOutputLatency;
		outp_param.hostApiSpecificStreamInfo = NULL;

		//todo listo para empezar a reproducir.
		PA_err = Pa_OpenStream(&stream, NULL, &outp_param, SAMP_FREQ, PA_BUFF_SIZ, paClipOff, playback_cb, &data);
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		PA_err = Pa_StartStream(stream);
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}

		//reproduciendo...
		std::cout << "REPRODUCING!! (" << SECONDS << " seconds)" << std::endl;
		while (Pa_IsStreamActive(stream) == 1) { //El loop termina cuando el arrglo de datos le dio todos sus datos a la salida.
			Pa_Sleep(100);
		}
		std::cout << "REPRODUCING FINISHED!! " << std::endl;
		PA_err = Pa_CloseStream(stream);
		if (PA_err != paNoError) {
			std::cerr << " Error in PA: " << Pa_GetErrorText(PA_err) << std::endl;
			Pa_Terminate();
			return -1;
		}
		//termino de reproducir.

		//pregunta al usuario si lo quiere volver a intentar.
		while (true) {
			std::cout << "DO YOU WANT TO RETRY?: ";
			std::cin >> option;
			if (option == "YES" || option == "yes" || option == "NO" || option == "no")
				break;
			else
				std::cout << "WHAT?" << std::endl;
		}
		if (option == "YES" || option == "yes")
			continue;
		else if (option == "NO" || option == "no")
			break;
	}

	Pa_Terminate();
	system("pause");
	return 0;
}