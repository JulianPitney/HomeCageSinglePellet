#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include "AVIRecorder.h"
#include <pthread.h>

#include<iostream>
#include<opencv2/highgui.hpp>
#include<opencv2/core.hpp>
#include<stdlib.h>
#include<chrono>


using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
using namespace cv;

// Number of nanoseconds in a second (Used for FPS calculation)
const int ns_per_second = 1000000000;
// Dimmensions of image being received
const int cols = 1000;
const int rows = 400;
// Toggle this to turn stream on and off
bool streaming = true;


enum aviType
{
	UNCOMPRESSED,
	MJPG,
	H264
};

const aviType chosenAviType = H264;


enum triggerType
{
	SOFTWARE,
	HARDWARE
};

const triggerType chosenTrigger = HARDWARE;




int initCameras(CameraList camList) {

	CameraPtr pCam = NULL;

	for (int i = 0; i < camList.GetSize(); i++)
	{
		// Select camera
		pCam = camList.GetByIndex(i);
		cout << "Initializing camera" + i << endl;
		// Initialize camera
		pCam->Init();
	}

}

int deinitCameras(CameraList camList) {

	CameraPtr pCam = NULL;

	for (int i = 0; i < camList.GetSize(); i++)
	{
		// Select camera
		pCam = camList.GetByIndex(i);
		cout << "Deinitializing camera" + i << endl;
		// Deinitialize camera
		pCam->DeInit();
	}
}

inline bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}


// This function configures the camera to use a trigger. First, trigger mode is
// set to off in order to select the trigger source. Once the trigger source
// has been selected, trigger mode is then enabled, which has the camera
// capture only a single image upon the execution of the chosen trigger.
int ConfigureTrigger(INodeMap & nodeMap) {

	int result = 0;

	cout << endl << endl << "*** CONFIGURING TRIGGER ***" << endl << endl;

	if (chosenTrigger == SOFTWARE)
	{
		cout << "Software trigger chosen..." << endl;
	}
	else if (chosenTrigger == HARDWARE)
	{
		cout << "Hardware trigger chosen..." << endl;
	}

	try
	{
		//
		// Ensure trigger mode off
		//
		// *** NOTES ***
		// The trigger must be disabled in order to configure whether the source
		// is software or hardware.
		//
		CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
		{
			cout << "Unable to disable trigger mode (node retrieval). Aborting..." << endl;
			return -1;
		}

		CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
		{
			cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << endl;
			return -1;
		}

		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

		CEnumerationPtr triggerSelector = nodeMap.GetNode("TriggerSelector");
		triggerSelector->SetIntValue(triggerSelector->GetEntryByName("AcquisitionStart")->GetValue());

		cout << "Trigger mode disabled..." << endl;

		//
		// Select trigger source
		//
		// *** NOTES ***
		// The trigger source must be set to hardware or software while trigger
		// mode is off.
		//
		CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
		if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
		{
			cout << "Unable to set trigger mode (node retrieval). Aborting..." << endl;
			return -1;
		}

		if (chosenTrigger == SOFTWARE)
		{
			// Set trigger mode to software
			CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
			if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
			{
				cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
				return -1;
			}

			ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

			cout << "Trigger source set to software..." << endl;
		}
		else if (chosenTrigger == HARDWARE)
		{
			// Set trigger mode to hardware ('Line0')
			CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line0");
			if (!IsAvailable(ptrTriggerSourceHardware) || !IsReadable(ptrTriggerSourceHardware))
			{
				cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
				return -1;
			}

			ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());

			cout << "Trigger source set to hardware..." << endl;
		}

		//
		// Turn trigger mode on
		//
		// *** LATER ***
		// Once the appropriate trigger source has been set, turn trigger mode
		// on in order to retrieve images using the trigger.
		//

		CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
		if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
		{
			cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
			return -1;
		}

		ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());
		// TODO: Blackfly and Flea3 GEV cameras need 1 second delay after trigger mode is turned on
		//unsigned int microseconds = 2000000;
		//usleep(microseconds);

		CEnumerationPtr triggerActivation = nodeMap.GetNode("TriggerActivation");
		triggerActivation->SetIntValue(triggerActivation->GetEntryByName("LevelHigh")->GetValue());


		cout << "Trigger mode turned back on..." << endl << endl;
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;



}


// This function returns the camera to a normal state by turning off trigger
// mode.
int ResetTrigger(INodeMap & nodeMap)
{
	int result = 0;

	try
	{
		//
		// Turn trigger mode back off
		//
		// *** NOTES ***
		// Once all images have been captured, turn trigger mode back off to
		// restore the camera to a clean state.
		//
		CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
		if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
		{
			cout << "Unable to disable trigger mode (node retrieval). Non-fatal error..." << endl;
			return -1;
		}

		CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
		if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
		{
			cout << "Unable to disable trigger mode (enum entry retrieval). Non-fatal error..." << endl;
			return -1;
		}

		ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

		cout << "Trigger mode disabled..." << endl << endl;
	}
	catch (Spinnaker::Exception &e)
	{
		cout << "Error: " << e.what() << endl;
		result = -1;
	}

	return result;
}


int AcquireImages(CameraPtr pCam, INodeMap &nodeMap, INodeMap &nodeMapTLDevice, char *vidPath) {

int result = 0;

cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

pCam->Height.SetValue(400);
pCam->Width.SetValue(1000);
pCam->OffsetX.SetValue(200);
pCam->OffsetY.SetValue(350);
pCam->AcquisitionFrameRateEnable.SetValue(true);
pCam->AcquisitionFrameRate.SetValue(float(135.00));

CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());
CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());
CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
exposureTime->SetValue(250);



// Retrieve device serial number for filename
		string deviceSerialNumber = "";

		CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
		if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
		{
			deviceSerialNumber = ptrStringSerial->GetValue();

			cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
		}

		//
		// Get the current frame rate; acquisition frame rate recorded in hertz
		//
		// *** NOTES ***
		// The video frame rate can be set to anything; however, in order to
		// have videos play in real-time, the acquisition frame rate can be
		// retrieved from the camera.
		//
		CFloatPtr ptrAcquisitionFrameRate = nodeMap.GetNode("AcquisitionFrameRate");
		if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
		{
			cout << "Unable to retrieve frame rate. Aborting..." << endl << endl;
			return -1;
		}
		float frameRateToSet = static_cast<float>(ptrAcquisitionFrameRate->GetValue());
		cout << "Frame rate to be set to " << ptrAcquisitionFrameRate->GetValue() << "...";



try
{
	// Set acquisition mode to continuous
	CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
	if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
	{
		cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
		return -1;
	}

	CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
	if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
	{
		cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl << endl;
		return -1;
	}
	int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
	ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
	cout << "Acquisition mode set to continuous..." << endl;

	//
	// Select option and open AVI filetype
	//
	// *** NOTES ***
	// Depending on the filetype, a number of settings need to be set in
	// an object called an option. An uncompressed option only needs to
	// have the video frame rate set whereas videos with MJPG or H264
	// compressions should have more values set.
	//
	// Once the desired option object is configured, open the AVI file
	// with the option in order to create the image file.
	//
	// *** LATER ***
	// Once all images have been added, it is important to close the file -
	// this is similar to many other standard file streams.
	//
	AVIRecorder aviRecorder;

	// Set maximum AVI file size to 2GB.
	// A new AVI file is generated when 2GB
	// limit is reached. Setting maximum file
	// size to 0 indicates no limit.
	const unsigned int k_aviFileSize = 2048;
	aviRecorder.SetMaximumAVISize(k_aviFileSize);

	if (chosenAviType == UNCOMPRESSED)
	{
		AVIOption option;
		option.frameRate = frameRateToSet;
		aviRecorder.AVIOpen(vidPath, option);
	}
	else if (chosenAviType == MJPG)
	{
		MJPGOption option;
		option.frameRate = frameRateToSet;
		option.quality = 75;
		cout << "Opening recorder...\n";
		aviRecorder.AVIOpen(vidPath, option);
		cout << "Done opening recorder...\n";
	}
	else if (chosenAviType == H264)
	{
		H264Option option;
		option.frameRate = frameRateToSet;
		option.bitrate = 8000000;

		option.height = static_cast<unsigned int>(400);
		option.width = static_cast<unsigned int>(1000);
		aviRecorder.AVIOpen(vidPath, option);
	}

	// Begin acquiring images
	pCam->BeginAcquisition();

	cout << "Acquiring images..." << endl;

	// Retrieve device serial number for filename
	gcstring deviceSerialNumber("");

	CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
	if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
	{
		deviceSerialNumber = ptrStringSerial->GetValue();

		cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
	}
	cout << endl;




	string pythonMsg;
  namedWindow("PtGrey Live Feed", WINDOW_AUTOSIZE);

	while(pythonMsg != "TERM")
	{

		if(file_exists("KILL"))
		{
			break;
		}

		try
		{
			ImagePtr pResultImage = pCam->GetNextImage();

			if (pResultImage->IsIncomplete())
			{
				cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
				pResultImage->Release();
			}
			else
			{

				void* img_ptr = pResultImage->GetData();
				Mat img(rows, cols, CV_8UC1, img_ptr);
				imshow("PtGrey Live Feed", img);
		    waitKey(1);
				aviRecorder.AVIAppend(pResultImage);
				pResultImage->Release();
			}
		}
		catch (Spinnaker::Exception &e)
		{
			cout << "Error: " << e.what() << endl;
			result = -1;
		}

	}

	// End acquisition
	pCam->EndAcquisition();
	aviRecorder.AVIClose();
}
catch (Spinnaker::Exception &e)
{
	cout << "Error: " << e.what() << endl;
	result = -1;
}

return result;

}



int main(int argc, char** argv) {

	cout << "PTGREY BOOTING...\n";

	// Retrieve singleton reference to system object
	SystemPtr system = System::GetInstance();
	// Retrieve list of cameras from the system
	CameraList camList = system->GetCameras();

	unsigned int numCameras = camList.GetSize();
	cout << "Number of cameras detected: " << numCameras << endl << endl;
	// Finish if there are no cameras
	if (numCameras == 0)
	{
		// Clear camera list before releasing system
		camList.Clear();

		// Release system
		system->ReleaseInstance();

		cout << "Not enough cameras!" << endl;
		cout << "Done! Press Enter to exit..." << endl;
		getchar();

		return -1;
	}


	initCameras(camList);
	// Retrieve GenICam nodemap for each camera
	INodeMap & nodeMap = camList.GetByIndex(0)->GetNodeMap();
	// Retrieve TL device nodemap for each camera
	INodeMap & nodeMapTLDevice = camList.GetByIndex(0)->GetTLDeviceNodeMap();
	// Configure Trigger for each camera
	//ConfigureTrigger(nodeMap);
	// Begin acquisition
	AcquireImages(camList.GetByIndex(0), nodeMap, nodeMapTLDevice, argv[1]);



	// Rest trigger
	ResetTrigger(nodeMap);
	// Deinitialize cameras
	deinitCameras(camList);
	// Clear camera list before releasing system
	camList.Clear();
	// Release system
	system->ReleaseInstance();

	return 0;
}
