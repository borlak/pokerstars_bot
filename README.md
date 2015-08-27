Many years ago when playing poker online in the USA was still legal, I decided to try and make a
poker bot.  I chose Poker Stars as my first target and began my quest on the creation of this bot.
Along the way I learned about: DLL injection, trampolining, and basic windows programming.

I never fully completed the bot, but in its current state it was capable of:<br>
1) Logging in<br>
2) Getting a list of tables<br>
3) Join a table with players but that is not full<br>
4) Click on betting buttons<br>

But I never got around to writing the actual betting logic.  Eventually, I learned that Poker Stars
started fighting back against botters like this one that used DLL injection, by renaming their 
internal functions and such.  Also you will notice I named the project "test2", as an attempt to
skirt bot detection by checking out running program names like "pokerstars-bot.exe".

This bot will probably not work at all with current day Poker Stars.  It relied on some things like
the table-list window size having particular proportions.  From what I remember, Poker Stars didn't
use the default Windows classes -- they inherited them and changed things around, I assumed to make 
it harder to identify things like which sub-window was the table-list.  I got around this by checking 
dimension proportions compared to the main window size.  Since the table-list window took up XYZ% of 
the main window, I could just loop through all the sub-windows and find it that way.

Have fun!
