# WordClock
An RGB word clock with built in web-server. 


<p align="center">
<img src="/figures/clocktwo.JPG" width="400px">
</p>

# Web Configuration
If it is your first time configuring your clocktwo, it will open a password-free WiFi access point named _clocktwo_. 
Connect to it via your PC or mobile.

Head over to [clocktwo.local](clocktwo.local) in your browser. 
After the website has loaded (this might take some time), you should see the following configuration.

<p align="center">
<img src="/figures/webserver.png" width="400px">
</p>

## General
```Name:```
 Give your clock a unique name. This comes in handy, if you have multiple of them. If you change the name, the website will be available under _<newName>.local_ and the WiFi access point will be renamed accordingly.
 
  ```Night Start:```
  Time when night mode will be active (24h format)
  
  ```Night Stop:```
  Time when night mode will be inactive (24h format)
  
  ```Time:```
  If your clocktwo is not connected to the internet, it might have a wrong date and time setting. You can adjust for this here. As the clock recognizes German Daylight Saving Time (DST), you may need to adjust it for your timezone. 
  
  ```Time Server:```
  The NTP time server to synchronize the precise date and time (requires internet connection).
  
  ```MQTT Server:```
  For integration into a smart home system. Allows for setting color and configuration over MQTT. 
  
## Network Configuration
 
  The clock can connect to an existing WiFi network for time synchronization and integration into existing smart home setups.
  As soon as it is succesfully connected to the network, it will stop opening an access point. 
  
   ```SSID:```
  Name of your WiFi.
  
   ```PWD:```
  Password of your WiFi.
  
  ## Color Configuration
  
   ```Color:```
 Color of the time displayed.
  
   ```Back Color:```
 Color of all other letters.

  
   ```Night Color:```
 Color of the time during _night mode_.
  
  ```Brightness:```
Birghtness slider in percent.
  
  ```Color Order:```
  If your clock uses LED stripes of different color order, you can adjust for it here. The clock will automatically restart once this setting is adjusted.
  
 
