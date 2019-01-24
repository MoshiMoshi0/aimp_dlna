#pragma once

class AimpHttp {
private:
	static IAIMPCore *core;
	static IAIMPServiceHTTPClient* httpService;

public:
	static HRESULT Initialize(IAIMPCore *Core) {
		core = Core;

		if (FAILED(core->QueryInterface(IID_IAIMPServiceHTTPClient, reinterpret_cast<void**>(&httpService)))) {
			return E_FAIL;
		}
		return S_OK;
	}

	static void Finalize() {
		if (httpService) {
			httpService->Release();
			httpService = nullptr;
		}

		core = nullptr;
	}

	static HRESULT DownloadImage(wstring url, IAIMPImageContainer **Image) {
		IAIMPMemoryStream* stream;
		if (SUCCEEDED(AimpUtils::CreateObject(IID_IAIMPMemoryStream, reinterpret_cast<void **>(&stream)))) {
			if (FAILED(AimpUtils::CreateObject(IID_IAIMPImageContainer, reinterpret_cast<void **>(Image))))
				return E_FAIL;

			uintptr_t* taskId;
			if (FAILED(httpService->Get(AimpString(url), AIMP_SERVICE_HTTPCLIENT_FLAGS_WAITFOR, stream, nullptr, nullptr, reinterpret_cast<void **>(&taskId))))
				return E_FAIL;

			unsigned int s = (unsigned int) stream->GetSize();
			byte* buf = new byte[s + 1];
			buf[s] = 0;
			stream->Seek(0, AIMP_STREAM_SEEKMODE_FROM_BEGINNING);
			stream->Read(buf, s);

			(*Image)->SetDataSize(s);
			memcpy((*Image)->GetData(), buf, s);

			stream->Release();
		}

		return S_OK;
	}
};

