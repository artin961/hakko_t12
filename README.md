Soldering IRON controller fot hakko t12 tips based on atmega328p-pu muicrocontroller (Arduino UNO/NANO)

## Story
In this project i will show you how to create your own soldering iron controller for amazing Hakko T12 tips on Arduino. This is my first soldering controller for hakko T12 tips.

The major update was performed 04/14/2020:

Support for optional TILT/REED switch added.
Support for optional ambient temperature sensor added.
Calibration procedure algorithm updated.
New menu items added.
Up to 42 tips can be activated from the whole-tips list using activation menu. You can select the tips you are going to use with this controller.
You have to update your hardware according to the updated schematics if you want use ambient temperature sensor or TILT/REED switch. Or you can continue to use your existing hardware. Please, see the detailed description below.


## The controller is working
The Hakko T12 soldering tips are amazing tools for soldering: they heat very fast and can reach very high temperatures in a short period of time. Each tip have a built-in thermo couple to control its temperature very accurately. Also, using appropriate handle, you can replace the tip on the fly in 1 second.

The main features of this controller are:

PID algorithm used to manage the supplied power. This allows you to keep the required temperature of the tip very accurate.
The controller supports individual calibration of the tip by three reference points: 200, 300 and 400 degrees Celsius.
There is a calibration procedure in the controller simplifying the tip calibration process. The calibration procedure has been updated 14/04/2020.
The controller uses high-frequency PWM signal to control the supplied power and controller is working silently.
Accelerated rotary encoder algorithm implemented in the controller. While the encoder rotated slowly, the temperature preset changes by 1 degree and if you turn the knob fast, the temperature preset would be increased by 5 degrees.
The temperature can be displayed in two degrees: Celsius or Fahrenheit.
The controller supports TILT switch in the handle or REED switch in the soldering station base to activate low power (standby) mode.
The Hakko T12 soldering tips

The first thing you need to use these tips is a handle. There are a several handles for these tips in the market and I would like to recommend you to order the FX-9501 one. This handle has a light weight, very convenient and has a good quality socket (inside) to plug the tip. You can easily change the tip on the fly if you wish.

The tip has 3 wires: "+", "-" and earth ground. The heater is connected consequently with the thermo couple inside the tip, so it required two wires to be connected to the controller only.

First problem: the connector
First problem was the original connector of the handle. It is so rarely used, I could not find the socket for it, so I used the aviation plug GX12-5.

## Connect the sensor
As I mentioned before, the tip uses two wires to heat and to check the temperature. The controller puts some power to the IRON for some period then turns the power off and checks the tip temperature. The thermo couple generates very small voltage, about several millivolts, so to check this voltage operating amplifier is implemented in the schematics. But the amplifier does not like a 24 volts on its input and could be destroyed. Hopefully, this problem has been already solved by qualified people: they used Zener diode to limit the voltage on the input circuit. You can see the sensor part of the schematics on the picture below.

The main component of the schematics is a dual channel operating amplifier, ad822. It has a mosfet transistors on its input, is rail-to-rail and has very low noise. I have bought couple of amplifiers on eBay supplier for $5 each. Now you can find much more cheaper fake variant of this amplifier on Chinese suppliers. Unfortunately, this variant is much noisy. So it is better to use another non-expensive amplifier, mcp602 for this controller. Also, you can use ad823 op-amp as well.

The temperature of the iron is checked by the first amplifier on pins 1, 2, 3 and the current through the iron is checked by the second amplifier on pins 5, 6, 7. Checking the current through the IRON allows to ensure that the tip is connected to the controller. When the tip removed from the handle the controller starts 'change tip' procedure allowing you to select new tip from the tip list menu.

## Supplying power to the iron
The first idea of supplying power to the iron was to use PWM signal by analogWrite() function of Arduino. Unfortunately, the default frequency of the Arduino PWM signal is 490 Hz and the iron makes a noise while heating. It was ridiculous. To make the iron silent the high-frequency PWM signal implemented on TIMER1 running 31250 Hz to power the IRON. To allow the FET managing such high frequency PWM signal, the three transistor mosfet driver is implemented in the schematics.

For short, the FET has near zero resistance while it is open and about infinite resistance when it is closed. But in the intermediate state (while the FET is switching) the resistance become valuable and the FET starts heating. Because the FET has capacitance in the gate that requires time to charge or discharge. This forum thread explains the problem in details.

Zener diode (18V) is used to limit the Gate voltage (Vgs) of the FET while it is open because the power supply voltage is higher than maximum value of mosfet vgs voltage. The fast recoverable diode FR104 removes the power from the iron when the FET is closed.

You can replace this diode with another one, but you must use fast recoverable diode.

The inductance shown in the schematics is a tor of outer diameter about 1.5 cm wired with 20 cm long copper. The inductance is about 50-100 uH. This inductance prevents flickering of the electricity while the iron is heating.

Support for TILT/REED switch and ambient temperature sensor
Support for hardware Tilt or Reed switch was added in the firmware 14/04/2020. This feature allows to turn the IRON into standby mode in case of IRON inactivity or when IRON has been put into base.

The tilt switch can be put into soldering iron handle. When the iron was laying still for specified period of time, the standby mode would be activated.

The reed switch can be placed into the iron base, and should be shorten when the iron put on the base. In this case, you can setup lower value for standby mode timeout.

Low temperature and timeout can be setup through menu.

Ambient temperature sensor NTC-type of 10kOhm is supported by the firmware. You can install it into the iron handle also.

## Build the controller
The complete controller schematics is shown on the picture below. The controller is built on the atmega328p-pu micro controller running at 16 MHz. You can use Arduino nano or UNO board if you wish. They are completely compatible.



There are some sockets that should be explained in the schematics. SV1 is a serial connector allowing load the sketch to the Arduino. You can use USB to UART converter to program the controller. JP1 is a lcd0802 connector. JP2 & JP3 are actually a single 5-pin aviation plug used to connect soldering iron handle. SV2 connector is used to attach rotary encoder.

As you can see on the picture, the DC-DC converter is used to get 5v for the operating amplifier and to power the micro controller. The WSTECH DC-DC isolated converter is implemented. It supplies very stable power for the operating amplifier.

This DC-DC converter could be replaced with another one DC-DC converter by ND like the one shown on the picture below

In case you failed to find any of isolated DC-DC converter, it is possible to replace it with non-isolated DC-DC 24v->5v converter + isolated DC-DC converter b0505s 5v->5v. See pictures below.

The software sketch can be downloaded from the GitHub repository.

Tune the controller
Note: The external thermo couple required to tune the potentiometer. To perform tuning procedure more precisely, apply a solder drop to the contact between the thermocouple and the soldering tip rod.

In the controller schematics you can see the potentiometer R10 (500kOhm) that tunes the operation amplifier to get the expected temperature readings at Arduino pin A0. You can use different operating amplifier, you can use different tips so this potentiometer increases the controller flexibility. This potentiometer should be tuned at least once when you created the controller. The main idea is that the controller reads the voltage from the thermo couple through the ADC and gets some integer data in the interval 0-1023 depending of the voltage on pin A0. The near the voltage to 5v the near the readings to the 1023. The potentiometer should be tuned so when the iron temperature become 400 centares, the ADC reading should be about 900.

To tune the potentiometer, select 'tune' menu item from the setup menu. The iron starts heating. The controller displays the temperature readings from A0 pin in the internal units (0-1023) in this mode. By rotating the encoder you can adjust the supplied power to the iron. Try to keep the iron temperature as near to 400 degrees of centigrade as possible (use external thermometer). At the beginning you can increase the supplied power to the maximum value to speed up the iron heating, then decrease the power to keep the temperature at the 400 degrees. When you stabilize the iron temperature near 400 degrees, rotate the multi-turn potentiometer handle to get the sensor readings near 900. Ensure that this readings is not the maximum one, rotate the variable potentiometer and get 930-950, then return back to 900. It is important because the controller should be able to measure the temperature greater than 400 degrees of Centigrade.

When the variable potentiometer is adjusted, press and hold the encoder handle for about 2 seconds. The initial setup procedure now is complete.

Note: the sketch assumes that the working temperature interval of the soldering iron is 180 - 400 degrees of Celsius. It is possible to change this interval by editing constants in the program sketch before you plan to tune the controller.

It is recommended to use the thick tip that produces the highest voltage to perform the tune procedure. For example, T12-K, T12-D52 or so.

The controller modes
The controller has several modes:

## Standby mode
Main working mode (keep the preset temperature)
Power mode (keep the supplied power)
Setup mode
Calibration mode
Tune mode (the initial potentiometer setup procedure)
When the controller is just powered on, the standby mode is activated. In this mode the soldering iron is powered off. The main display shows the following information:

The preset temperature on the left side of the upper line (in units selected - Celsius or Fahrenheit);
The 'OFF' message on the right side of the upper line indicating that the iron is powered off;
The current temperature of the iron is on the left side of the second line;
In case the iron tip is not connected to the controller, the controller starts tip selection mode. Select the tip name you are going to insert from the tip list, then insert the tip.

Note: In the update firmware version, up to 42 soldering iron tips can be activated and used, but the whole tip list supported. First, you have to select (activate) the tips you are going to use. To do so, select "activ." menu item.

The preset temperature can be adjusted by rotating the encoder handle while the iron is powered off. To power on the soldering iron, push the encoder handle lightly. The main working mode of controller will be activated. Now the controller keeps the iron temperature near the preset one.

In the main mode the display shows the following information:

The preset temperature on the left side of the upper line (in units selected - Celsius or Fahrenheit)
The 'ON' message on the right side of the upper line indicating that the iron is powered on and is still heating. When the iron reaches the preset temperature, the 'rdy ' (ready) message will be displayed; When the controller activates the standby mode, the 'stb' (standby) message will be displayed.
The current temperature of the iron is displayed in the left side of the second line;
The power supplied to the iron (in percents of the maximum allowable power) is displayed in the right side of the screen;
By rotating the encoder it is possible to change the preset temperature. The message 'ON' would be showed again till the iron would reached the new preset temperature. To return to the standby mode, press the encoder handle lightly.

In the main mode, the controller checks that the iron is working properly.

The power mode can be switched on by long pressing the encoder while it is in the main mode. In the power mode, you can adjust the power supplied to the soldering iron manually by rotating encoder. In the power mode the screen shows the power supplied to the iron in the internal units and the iron temperature in human readable units. By pressing the encoder toggles on-off power in the power mode. Long press of the rotary encoder handle will return the controller from power mode to the main working mode.

## The controller menu
To get to the controller setup menu, long press the encoder in standby or tip selection mode. In the setup mode the configuration parameters can be adjusted. There are 11 menu entries in this mode:

temperature units (Celsius or Fahrenheit)
buzzer (ON/OFF)
Switch type: tilt/reed
Ambient temperature sensor (enabled/disabled)
Standby temperature (if zero, disables automatic low-power mode)
Standby timeout (seconds)
automatic power-off timeout (minutes)
the iron calibration ('calib.')
the iron tips activation ('activ.')
initial setup procedure ('tune')
save the changes
cancel the changes
Rotate the handle to select the menu item. To change the selected item, press lightly on the encoder handle. After adjusting the parameter, press the handle again to return to the setup menu. Long press of the encoder handle can be used to return back to standby mode and save the parameters in the EEPROM.

## Activate soldering tips
The controller supports near all tips available in the world, if not, you can add new tip at any time by editing iron_tips.cpp source file. It is not convenient to select next tip from the whole list, so you have to activate the tips you are going to use from the whole-tip list. To do so, select 'activ.' menu item from the setup menu, rotate encoder to select required tip and press the encoder to toggle tip activation. The controller saves the tip calibration data of all active tips in the EEPROM. Up to 42 active tips are supported by this firmware version.

To finish activation procedure, long press the encoder.

Note: The setup menu is available from standby mode or from 'tip selection' mode by long press the encoder.

## Calibrate the soldering tip
Note:The external thermocouple required to calibrate the iron tip. To perform calibration procedure more precisely, apply a solder drop to the contact between the thermocouple and the soldering tip rod. To heat-up the tip and apply soldering rod, you can use tune procedure (see above).

The controller reads the temperature in internal units (0-1023) by reading the voltage on A0 Arduino pin. It is convenient for us to use the human readable units like degrees of Celsius or Fahrenheit. There are three reference points for soldering iron calibration: 200, 300 and 400 degrees of Celsius. The controller saves internal readings for these three reference points and uses them to convert the temperature from internal reading to human readable values.

The calibration process requires to check the temperature in 7-8 points. Select 'calib.' menu item from the setup menu to start calibration process. The calibration screen would be displayed with 'OFF" message indicating the iron tip is ready to be calibrated. Switch-on the iron by pressing the encoder handle. The soldering iron starts heating.

The PID algorithm allows reach the desired temperature in short time. When the reference point temperature would be reached, the controller would 'beep' indicating it is ready to check the temperature by external sensor. Wait a little allowing the temperature to stabilize, then check the real temperature of the soldering iron by external sensor. Then rotate the handle of the rotary encoder and enter the real temperature of the iron. Press the encoder handle lightly. The controller saves the real temperature of the reference point and continues to check the temperature at next reference point. The reference point number would be displayed on the first display string. As soon as all measurements would be finished, the calibration procedure finished and the controller would return at main mode.

## Automatic power-off feature
The automatic switch-off feature required of using tilt or reed switch in the updated firmware version,

Updated 04/24/2020.
 - Fixed issue of accidentally switching from Celsius to Fahrenheit
 - No more checking for iron connectivity while it is active
 
Updated 04/18/2020. The ambient temperature support restored.
 - Fixed wrong temperature readings issue in keepTemp() procedure.
 - Fixed standby mode actiovation issue.

Updated 04/16/2020. To increase stability, removed suport for ambient temperature sensor.

Updated 04/14/2020. Major changes include schematics update.
- Support for optional TILT/REED switch added.
- Support for optional ambient temperature sensor added.
- Calibration procedure algotithm updated.
- New menu items added.

Because the major changes were made, the prevoius stable firmware version has been moved in OLD folder for backup.

Updated 09/05/2019. Preset temperature jumps when finished rotating the encoder issue fixed

Updated 04/18/2019. Two display configuration support added. You should select appropriate display in the config.h file

Updated 01/14/2018. Fixed bug with the IRON overheat. New procedure in the tip configutration implemented. 

Updated 11/23/2017. Fixed bug with cold temperature detection. New criteria for ready state implemented in the working mode and in the tip configuration mode.

Updated 10/21/2017. New PID parameters used. 

Updated 10/14/2017. Fixed unstable work with 'thin' soldering tips such as T12-BC1, T12-JL02, T12-IL, T12-D12 by Increasing the frequency of temperature sampling to 50 Hz. New PID coefficients implemented.

Updated 7/1/2017. New software features are available: new per-tip calibration and high-frequency temperature check by Timer1, now no addititional software library required. 
