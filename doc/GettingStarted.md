![Slide1](./images/Slide1.PNG)
---
![Slide2](./images/Slide2.PNG)
### In a new window, select Clone Git Repository and clone the following repo:

https://github.com/geappliances/home-assistant-adapter
---
![Slide3](./images/Slide3.PNG)
---
![Slide4](./images/Slide4.PNG)
---
![Slide5](./images/Slide5.PNG)
---
![Slide6](./images/Slide6.PNG)
---
Install MQTT explorer: https://mqtt-explorer.com/
![Slide7](./images/Slide7.PNG)
---
<img align="left" src="./images/Slide8.PNG">

In MQTT under geappliances and the device ID that you specified in config.h, you should see a list of ERDs.  You may need to power cycle the appliance for the list to be fully populated.

The next step is to generate a yaml configuration file that will define what ERDs appear as entities in Home Assistant.  ERD documentation can be found here:  https://github.com/geappliances/public-appliance-api-documentation/

Details on configuration of MQTT components via Yaml in Home Assistant can be found here: https://www.home-assistant.io/integrations/mqtt/

Some example yaml files can be found in the packages directory of the github repo.  To utilize the example files, transfer them to your Home Assistant instance (using Samba or other means) to /config/packages/

Let’s walk through a specific example of creating Yaml to expose a specific ERD…..
