# Patro
This is a terminal based Nepali calendar. Database containing date table is included.

#### Install
1.  Create a new database `mitidb`
2. `$ mysql -u <username> -p mitidb < miti.sql`
3. `$ make`
4. Copy `build/main` to executable directory or create an alias. <br>
  example: `alias patro='.../build/main'` in  your `.bashrc` or `.bash_aliases` file.

#### Usage
This program is terminal based, so the textual alignment may not the same in different systems.
* `$ patro`  <br>
  displays the calendar for current Nepali month
* `$ patro <year> <month>` <br>
  example: `$ main 2070 5` <br>
  displays calendar for given Nepali **month** of year **year**
