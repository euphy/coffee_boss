# coffee_boss

At work, there is a Bravilor Bonamat Novo 2 filter coffee machine.

I buy batches of ground coffee every month or so, and ask drinkers to fund this by putting money into a cup nearby. 

I harvest the money from the cup every few days and mark how close we are to having enough money to buy a new batch 
with a totaliser on the side of the cup.

I want to be able to tell drinkers what they should be contributing. I know the cost per carafe, but I don't know
how many cups each carafe supplies. Some cups are huge, some are tiny. The people with big cups get a great deal, the
people with small cups subsidise them.

Read more here: http://www.euphy.co.uk/tag/coffee-boss/

## The plan

Add a device to the coffee machine that tells a drinker how much money to put into the pot.

A digital scale under the coffee machine that will log weight changes and be able to recognise:

  * Cup of coffee being taken (carafe lifted, returned lighter)
  * Size of cup taken (difference in weight)
  * Number of cups taken per carafe (used to calculate cost per cup)

And show:

  * What a cup of coffee costs lately
  * How your cup was different to average cups (+/-)
  * How old the coffee in the carafe is


## The environment

This is in a corporate office. I can't connect to any internal network, so this must be standalone. I can supply power OK.


## The implementation

I've got loads of ESP32s, LCDs (with SD card readers attached). I'll use one of my Polarshield v3 boards 
as the basis of it (because it wires up the LCD and SD already), supplement it with an RTC module and the 
[load cells](https://www.banggood.com/4pcs-DIY-50KG-Body-Load-Cell-Weight-Strain-Sensor-Resistance-With-HX711-AD-Module-p-1326815.html).
