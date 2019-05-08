# mailbox-sensor
Sensor for your mailbox

Now with power checking feature!

Main point of this device is to show me there is something in my mailbox. I'm unable to see inside mailbox without unlocking it and opening. Simple LED is enough for me, but you can replace it with servo lifting flag, speakers going of, wifi module sending you a message and etc.

This code is made to work with ATtiny85. Uses two IR emitters (can be used only one), one IR receiver, one LED, one button.
Set pin numbers, sensitivity (by trial and error), checking interval and LED brightness in a beginning of the code.

Idea of a code:
ATtiny85 sleeps for set time (although it wakes up every ~8s due to limitation of sleep abilities of ATtiny85), wakes up, measures IR amount, measures IR amount with IR emitters turned on (one by one), compares measurements (takes into account sensitivity setting) and decides if there is something between IR emitter/receiver. If so, turns on LED for you to see that there is a letter and wait for you to take it and press button which starts loop from the beginning. If not, starts loop from the beginning.
If you want to check if batteries still alive press and hold (up to ~8s) button while LED is off. LED should light up for couple seconds showing there is juice left in batteries.

It's not ideal, but it works. Been using for couple months and batteries still in good shape so no need to check it every week.

Pro advice: hide LED not to be seen by walk passers. Light in mailbox can spark alarming ideas for them.
