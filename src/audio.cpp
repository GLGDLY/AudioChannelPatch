#include "audio.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QtLogging>


Audio::Audio() {
	qDebug() << "Creating new audio object";

	HRESULT hr;

	CoInitialize(NULL);

	IMMDeviceEnumerator* pEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
						  (LPVOID*)&pEnumerator);
	if (FAILED(hr)) {
		qDebug() << "Failed to create instance of IMMDeviceEnumerator";
		throw hr;
	}


	IMMDevice* pDevice = NULL;
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (FAILED(hr)) {
		qDebug() << "Failed to get default audio endpoint";
		throw hr;
	}

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (LPVOID*)&pAudioEndpointVolume);
	if (FAILED(hr)) {
		qDebug() << "Failed to activate IAudioEndpointVolume";
		throw hr;
	}

	hr = pAudioEndpointVolume->GetChannelCount(&channels);
	if (FAILED(hr)) {
		qDebug() << "Failed to get channel count";
		throw hr;
	}

	qDebug() << "Channel count: " << channels;

	for (int i = 0; i < channels; i++) {
		ori_level.push_back(this->get_channel_vol_level_scalar(i));
		qDebug() << "Channel " << i << " original volume level: " << ori_level[i] * 100 << "%";
	}
}

Audio::~Audio() {
	qDebug() << "Destroying audio object";

	this->reset_all_channel_vol_level_scalar();

	pAudioEndpointVolume->Release();
	CoUninitialize();
}

UINT Audio::get_channels() { return channels; }

float Audio::get_channel_vol_level_scalar(UINT channel) {
	float level;
	HRESULT hr = pAudioEndpointVolume->GetChannelVolumeLevelScalar(channel, &level);
	if (FAILED(hr)) {
		qDebug() << "Failed to get channel volume level scalar";
		throw hr;
	}

	return level;
}

void Audio::set_channel_vol_level_scalar(UINT channel, float level) {
	qDebug() << "Setting channel " << channel << " volume level to " << level * 100 << "%";
	const int retry = 3;
	HRESULT hr;
	for (int i = 0; i < retry - 1; i++) {
		hr = pAudioEndpointVolume->SetChannelVolumeLevelScalar(channel, level, NULL);
		if (FAILED(hr)) {
			qDebug() << "Failed to set channel volume level scalar" << i + 1 << " times";
			throw hr;
		}
	}
	hr = pAudioEndpointVolume->SetChannelVolumeLevelScalar(channel, level, NULL);
	if (FAILED(hr)) {
		qDebug() << "Failed to set channel volume level scalar" << retry << " times";
		throw hr;
	}
}

void Audio::reset_all_channel_vol_level_scalar() {
	for (int i = 0; i < channels; i++) {
		this->set_channel_vol_level_scalar(i, ori_level[i]);
	}
}

void Audio::save_ori_level() {
	for (int i = 0; i < channels; i++) {
		ori_level[i] = this->get_channel_vol_level_scalar(i);
	}
}

void Audio::print_vol_level_scalar() {
	std::cout << "===============";
	for (int i = 0; i < channels; i++) {
		float level = this->get_channel_vol_level_scalar(i);

		std::cout << "Channel " << i << " volume level: " << level * 100 << "%";
	}
}