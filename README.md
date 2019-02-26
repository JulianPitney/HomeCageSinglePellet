#**Overview**:

This system allows the user to host up to 5 mice in their home environment and automatically administer the single pellet reaching test to those animals. The system can run unsupervised and continuously for weeks at a time, allowing all 5 mice to perform an unlimited number of single pellet trials at their leisure. 

The design allows a single mouse at a time to enter the reaching tube. Upon entry, the animal’s RFID tag will be read, and if authenticated, a session will start for that animal. A session is defined as everything that happens from the time an animal enters the reaching tube to when they leave the tube. At the start of a session, the animal’s profile will be read and the task difficulty will be automatically adjusted by moving the pellet presentation arm to the appropriate distance away from the reaching tube. After the difficulty is set, pellets will begin being presented with either the left or right presentation arm, depending on which arm is specified in the mouse’s profile. Pellets will continue to be presented periodically until the mouse leaves the tube, at which point the session will end. Video and other data is recorded for the duration of each session. At session end, all the data for the session is saved in an organized way. 





#**Dependencies:**
* Ubuntu v16.04 LTS: Kernel version 4.4.19-35 or later
* Python v3.5.2
	* pySerial	
	* numpy
	* OpenCV v3.4.x
	* tkinter 
	* matplotlib
* Flir Spinnaker SDK v1.10.31
	* libavcodec-ffmpeg56
	* libavformat-ffmpeg56
	* libswscale-ffmpeg3
	* libswresample-ffmpeg1
	* libavutil-ffmpeg54
	* libgtkmm-2.4-dev
	* libusb-1.0-0
	
	Note: The Spinnaker dependencies are installed via official Spinnaker SDK install script.
* Arduino IDE v1.8.5

#**Softwarre Installation:**
1. Install Ubuntu 16.04 LTS on your machine.
2. Install Anaconda. (https://www.anaconda.com/distribution/)
3. Install the Flir Spinnaker SDK v1.10.31 (https://www.ptgrey.com/support/downloads)
4. Install Arduino IDE v1.8.5. (https://www.arduino.cc/en/Main/Software)
5. Install OpenCV for C++.
	- `sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev`
	- `sudo apt-get install libjpeg-dev libpng-dev libtiff5-dev libjasper-dev libdc1394-22-dev libeigen3-dev libtheora-dev 			libvorbis-dev libxvidcore-dev libx264-dev sphinx-common libtbb-dev yasm libfaac-dev libopencore-amrnb-dev 			libopencore-amrwb-dev libopenexr-dev libgstreamer-plugins-base1.0-dev libavutil-dev libavfilter-dev 				libavresample-dev`
	- `sudo -s`
	- `cd /opt`
	- Download OpenCV source from https://github.com/opencv/opencv/releases/tag/3.4.4 and unpack it into /opt
	- Download OpenCV_contrib source from https://github.com/opencv/opencv_contrib/releases/3.4.4 and unpack it into /opt
	- `cd opencv`
	- `mkdir release`
	- `cd release`
	- `cmake -D BUILD_TIFF=ON -D WITH_CUDA=OFF -D ENABLE_AVX=OFF -D WITH_OPENGL=OFF -D WITH_OPENCL=OFF -D WITH_IPP=OFF -D 			WITH_TBB=ON -D BUILD_TBB=ON -D WITH_EIGEN=OFF -D WITH_V4L=OFF -D WITH_VTK=OFF -D BUILD_TESTS=OFF -D 				BUILD_PERF_TESTS=OFF -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D 						OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules /opt/opencv/`
	- `make j4`
	- `make install`
	- `ldconfig`
	- `exit`
	- `cd`
	- `pkg-config --modversion opencv`
	- If everything worked correctly, output should read "3.4.4".
	
5. Create and configure a virtual environment for installing the HomeCageSinglePellet code.
	- `conda create -n <yourenvname> python=3.5.2 anaconda`
	- `conda activate <yourenvname>`
	- `conda install -c conda-forge opencv=3.4.4`
	- `conda install -c anaconda numpy`
	- `conda install -c anaconda pyserial`
	- `conda install -c anaconda tk`
	- `conda install -c conda-forge matplotlib`

6. Download the HCSP source code from https://github.com/SilasiLab/HomeCageSinglePellet and unpack it.
7. Optional (Only if you want to use the analysis features): Install Deeplabcut using the Anaconda based pip installation method. (https://github.com/AlexEMG/DeepLabCut/blob/master/docs/installation.md)
8. Optional (Only if you want to use the anaylsis features): Move the file `HomeCageSinglePellet/src/analysis/HCSP_analyze.py` into `~/.conda/envs/DLC2/lib/python3.6/site-packages/deeplabcut` (Where DLC2 is the name of the anaconda virtual environment where you installed Deeplabcut).
8. Done!
	
	
#**Assembly:**

###**Attach headers and components to the PCB**

1. Attach female headers for the Arduino onto the PCB. Attach male headers onto the CAMTRIG, MSZ, SERVO1, SERVO2, RFID and
IRBREAK holes on the PCB.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP1-2.png">

2. Solder the stepper driver to the PCB in the orientation shown in the picture. (Note that the holes on 
the PCB are spaced too far apart for the stepper driver pins). To address this we bend the pins of the driver to fit. You'll 
need to do this until someone gets around to fixing the PCB layout.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP3.png">
	
3. Solder the capacitor across C1 as shown in the picture.

###**Install the voltage regulator and power connectors**

4. Attach 6 leads to the voltage regulator:
	- 2 Leads to the positive input pad.
	- 2 leads to the negative input pad.
	- 1 lead to the positive output pad.
	- 1 lead to the negative output pad.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP5.1.png">
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP5.2.png">
	
5. Short the neutral and ground leads on the circular power connectors by soldering a small wire across them.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP6.png">

6. Line up the voltage regulator in the box and drill holes through the box where you want to mount the regulator.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP7.png">

7. Screw one of the circular power connectors into the bottom hole.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP8.png">

8. Solder one of the negative input leads you attached to the voltage regulator in step 1 onto the negative lead of the 
power connector you screwed in in step 4.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP9.png">

9. Solder one of the positive input leads you attached to the voltage regulator in step 1 onto the positive lead of the
power connector you screwed in in step 4. 
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP10.png">

10. Place the PCB into the box and drill holes into the box where the mount holes on the PCB line up. 
Then mount the PCB with some bolts. Metal bolts are fine, they won't touch anything that will be a problem. 
Use nylon washers if you're nervous.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP11.png">

11. Solder the remaining positive input power lead that you attached to the voltage regulator in 
step 5 to the positive MOTORPWR pad on the PCB.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP12.png">

12. Solder the remaining negative input power lead that you attached to the voltage regulator in 
step 5 to the negative MOTORPWR pad on the PCB.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP13.png">

13. Screw in the second circular power connector to the remaining hole in the box. Solder a wire from the negative lead of the
second power connector to the negative lead of the first power connctor. Solder a wire from the positive lead of the second power
connector to the positive lead of the first power connector.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP14-16.png">

###**Assemble the pellet delivery system and mouse tube**

14. Line the hopper rail piece up on the baseplate and screw in the 4 screws that hold it in place.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP17.png">

15. Attach the stepper motor as shown in the picture. Attach the coupler to the shaft of the motor.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP18.png">
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP18.2.png">

16. Attach the screw to the stepper coupler. Slide the hopper onto the rails and insert the nut into the slot where the stepper coupler screw will go. Then thread the shaft coming out of the coupler into the nut. 
<img width="600" height="300" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP17_NEW.png">

17. Rotate the stepper coupler until the screw is all the way into the hopper nut. Solder two leads to the microswitch,
one each on the left and right prongs coming out of the switch. Use superglue to glue the microswitch down so it will be pressed
when the hopper reaches it's closest point to the motor. 
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP18_NEW.png">

18. Attach both front surface mirrors to the mirror mount piece with double sided tape.
<img width="900" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP21.png">

19. Slide the front-wall/mirror holder piece over the end of the acrylic tube.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP22.png">

20. Use your favorite adhesive to attach the IR-Breaker to the IR-Breaker holder.
<img width="900" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP23.png">

21. Slide the IR-Breaker holder over the tube so that the beam runs across the bottom of the tube 
(Where the mice feet will be).
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP22_NEW.png">

22. Hotglue the RFID reader to the acrylic tube as shown.(Note: Push the IR-Breaker holder up against the RFID module so that 
it's sandwiched between the IR-Breaker holder and the front-wall/mirror holder, and hot glue it all in place).
<img width="600" height="900" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP25.png">

23. Slot the assembled acrylic tube into the square hole on the rail baseplate mount as shown.
<img width="900" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP26.png">

24. Line up both servos as shown in the picture. Rotate the servo splines as far as they will go in the 
directions shown in the picture.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP27-28.png">

25. Use an allen key to insert the set screws into the tips of the hopper arms.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP29.png">

26. Attach the hopper arms to the servo spline as shown in the picture. (Note: This is why we 
rotated the servo spline as far as possible in step 28, so that you can set the maximum retraction 
point of the hopper arms by attaching them how you like).
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP30.png">

27. Line the servos with attached hopper arms up as shown in the picture and use your favourite adhesive to 
adhere them to the servo mount points on the hopper.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP31.png">

28. This step is where we will glue the acrylic tube permanently into the sqaure 3D printed hole it sits in. Rotate the stepper
coupler until the hopper is as close as it can get to the motor. Now rotate the hopper arms up so that they are sitting infront 
of the tube opening as shown. On bootup the stepper motor will "zero" the hopper using the limit switch. We want this zero point
to put our pellet presentation arms directly infront of the tube opening. Additionally we want the tube to sit at a high enough angle that the hopper will not catch on the mirrors or any part of the tube assembly when it moves. Once you're satisfied that both these requirements are met, glue the tube into place. 
<img width="800" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP29_NEW.png">

29. Splice the power wires of both servo motors together. Splice the ground wires of both servo motors together. Splice the servo power wire into the lead coming out of the voltage regulator positive output. Splice the servo ground wire into the lead coming out of the voltage regulator negative output.
<img width="600" height="800" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP14_NEW.png">

30. Plug the leftmost servo signal wire into the SERV02 signal pin on the PCB. Plug the rightmost servo signal wire into the 
SERVO1 signal pin on the PCB.
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP15_NEW.png">

31. Splice the power wires of the IRbreak LED and photo diode together. Splice the ground wires of the IRBreak LED and photo diode together. Plug the spliced power, spliced ground and signal line of the IRbreaker into the IRBREAK pins on the PCB. 
<img width="600" height="600" src="https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/STEP16_NEW.png">

32. Plug the microswitch wires into the MSZ pins on the PCB. (TODO: Figure out which wire goes onto which pin and add a picture)

33. Assembly complete!
 




#**Usage**:
###**Running the Device**
1. Enter the virtual environment that the system was installed in by typing `source activate <my_env>` into a terminal.

2. Use `cd` to navigate to HomeCageSinglePellet/src/client/ and then run `python -B genProfiles.py`. The text prompts will walk you through entering your new animals into the system.

3. Open HomeCageSinglePellet/config/config.txt and set the system configuration you want.

4. Enter HomeCageSinglePellet/src/client/ and run `python -B main.py`

5. To test that everything is running correctly, block the IR beam breaker with something
	and scan one of the system’s test tags. If a session starts properly, it’s working.

6. To shut the system down cleanly; 

	- Ensure no sessions are currently running. 
	- Press the quit button on the GUI.
	- Ctrl+c out of the program running in the terminal.



###**Analysis**:

A high level analysis script is provided that runs the data for all animals through a long analysis pipeline. The functions performed on each video include;

* Analyze with Deeplabcut2.
* Identify all reaching attempts and record frame indexes where reaches occur.
* Compute 3D trajectory of reaches and estimate magnitude of movement in millimeters.
* Cut videos to remove any footage not within the bounds of a reaching event.
* Neatly package data for each analyzed video in a descriptively named folder within the animals ./Analyses/ directory.

This analysis is started by entering the HomeCageSinglePellet/src/analysis/ directory and running
`bash analyze_videos.sh <CONFIG_PATH> <VIDEO_DIRECTORY> <NETWORK_NAME>`

This script takes the following input;

**CONFIG_PATH** = Full path to config.yaml file in root directory of DLC2 project.
![alt text](https://raw.githubusercontent.com/SilasiLab/HomeCageSinglePellet/master/resources/Images/CONFIG_PATH_ANALYSIS.png)

**VIDEO_DIRECTORY** = Full path to directory containing videos you want to analyze. (e.g <~/HomeCageSinglePellet/AnimalProfiles/MOUSE1/Videos/>)

**NETWORK_NAME** = Full name of DLC2 network you want to use to analyze videos. (e.g <DeepCut_res
net50_HomeCageSinglePelletNov18shuffle1_950000>)


Another script (`HomeCageSinglePellet/src/analysis/scoreTrials.py`) is provided for manually categorizing reaches once they have been identified in the step detailed above. This script opens a GUI that allows the user to select an animal and browse through all the videos that have been analyzed for that animal. (In the video list, blue videos indicate videos where reaches were detected by the analysis software. Beige videos indicate videos where no reaches were detected. Green videos indicate videos that have already been manually scored). When users select a video from the list, the video window will display the first detected reach in a loop. It will also display the "reach count" (e.g 1/16) to indicate how many reaches the video contains and which one is currently being viewed. The user can then use the mouse or a hotkey to place the current reach into a category. The video window will then jump to the next reach. This repeats until all the reaches for a given video are scored, at which point the category information will be saved. 

**Note:** All the analysis functions read and write all their data to/from `HomeCageSinglePellet/AnimalProfiles/<animal_name>/Analyses/`. Information for each video is saved in a unique folder whose name includes video creation date, animal RFID, cage number and session number. (e.g `2019-01-25_14:36:31_002FBE737B99_67465_5233`). Each of these folders will contain the raw video, the deeplabcut output from analyzing the video (.h5 and .csv formats). In addition, if >=1 reaches were found in the video, the folder will contain a file named date_time_rfid_cage_number_session_number_reaches.txt (e.g `2019-01-25_14:36:31_002FBE737B99_67465_5233_reaches.txt`). This file contains the start and stop frame indexes of each reach in the video and (x,y,z) vectors for each reach. In addition, once a video has been scored manually using `scoreTrials.py`, a file named date_time_rfid_cage_number_session_number_reaches_scored.txt (e.g `2019-01-25_14:36:31_002FBE737B99_67465_5233_reaches_scored.txt`) will be added to the video's folder. This file is the same as date_time_rfid_cage_number_session_number_reaches.txt, except that it also contains a category identifier for every reach.


#**Troubleshooting**:

* Is everything plugged in?
* Shutting the system down incorrectly will often cause the camera and camera software to enter a bad state.
	* Check if the light on the camera is solid green. If it is, unplug the camera and plug it back in.
	* Check the HomeCageSinglePellet/src/client/ directory for a file named KILL. If this file exists, delete it.
	* Make sure the camera is plugged into a USB 3.0 or greater port and that it is not sharing a USB bridge with too many 			other devices (I.e if you have 43 devices plugged into the back of the computer and none in the front, plug the 		camera into the front).
* Make sure you are in the correct virtual environment.
* Make sure the HomeCageSinglePellet/config/config.txt file contains the correct configuration. (If the file gets deleted it will be replaced by a default version at system start)
* Make sure there are 1 to 5 profiles in the HomeCageSinglePellet/AnimalProfiles/ directory. Ensure these profiles contain all 		the appropriate files and that the save.txt file for each animal contains the correct information. 


