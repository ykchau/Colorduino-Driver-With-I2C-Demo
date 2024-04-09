Copyright © 2024 YK Chau ( youtube.com/ykchau888 ). All rights reserved.


# Colorduino Driver with I2C communication Demo
This is part of the LaLiMat project (https://www.youtube.com/playlist?list=PLJBKmE2nNweRXOebZjydkMEiq2pHtBMOS in Chinese), which is a multi-function Pixel Display, similar to LaMetric Time/Awtrix.

I had planned to release the entire project on YouTube, but I didn't have time to create the videos, so the work was abandoned. I don't want to waste the work I've done, so I uploaded this to GitHub, this is not the final version that you see on youtube, because the code is too long and hard to explain without step by step, so I uploaded the basic version, hoping it will help someone who is looking for a solution for Colorduino.

# Why re-invent the wheel?
There are already some people who have posted their work on Colorduino on the internet. However, most of the code is based on the original driver, which doesn't work with I2C. This means you can't connect multiple Colorduinos and communicate with each other. Also, the original driver seems to have issues in certain situations, which is why the wheel was reinvented.

# What is this driver for
This is a driver for Colorduino (or Funduino), which allows communication with multiple Colorduinos in a serial connection using I2C. You need to load the driver onto the Colorduino.

# Are there any doc for this driver
The short answer is NO. I am sharing this on GitHub because I don't have time to do additional work. However, I have added sufficient comments in the code. If you still have questions, you can ask, but please don't expect a reply (because I have already forgotten what I did before :P, sorry for that).

# What I need to make this code work
Hardware
- ESP8266 (I use WEMOS D1 R2), if you are doing some small project, you can use Colorduino itself as a master device, however, because the memory size is too small, I still recommend you use a standalone controller to do the master job.
- FTDI FT232RL (to upload the driver/program to Colorduino)
- USB ISP (to upload the original bootloader to the board)
- Colorduino/Funduino
- 8x8 LED Matrix

Others
- VS Code recommended
- Internet access for NTP (you can ignore it if you don't need it)

# How to use these codes
1. You need to assign each Colorduino with a unique I2C address, and then upload it to the specific Colorduino board
   - I use adress 0x00 of the EEPROM for store the I2C address
   - For assign the I2C address, open the slave.ino, search //EEPROM.write(0, 0x70); , uncomment it, change the 0x70 to the address you want.
   - Upload the program to Colorduino
   - You only need to do this at the FIRST time, so please comment is out after
2. Upload master.ino to master(your esp or Arduino board, WEMOS D1 R2 in this case), and slave.ino to slave device (Colorduino)
3. That's it.

# What should I see after uploading the code
1. A marquee of time and message should be displayed on the Colorduino, you can change what is to be displayed on the loop() in master.ino
2. If you want to change the displaying message, go to line 288 and change it.
3. I also make some social media logos, you can display them by uncommenting line 290, and commenting out line 289 of master.ino

# How I say thanks if I found this useful
- Give me a star
- Buy me a coffee https://www.paypal.com/paypalme/ykchau
- Subscribe to my Youtube channel at https://www.youtube.com/@YKChau888/ (and, of course, click on some ads, HAHAHAHA)
- Please inform me if you are using this on a commercial project.

Thanks
