#!/usr/bin/expect -f

set timeout 10

spawn ./menu

expect "Enter a number that corresponds to your choice > "
send -- "3\r"

expect "Enter a number that corresponds to your choice > "
send -- "2\r"

expect "Enter customer number > "
send -- "141\r"

# page 1
expect "1969 Harley Davidson Ultimate Chopper 66"
expect "1969 Corvair Monza 125"
expect -- "-- Page 1 -- "
send -- ">\r"

# page 2
expect "1968 Dodge Charger 59"
expect -- "-- Page 2 -- "
send -- "<\r"

# back on page 1
expect "1969 Harley Davidson Ultimate Chopper 66"
expect -- "-- Page 1 -- "
send -- "q\r"

puts "\n--------------------------------OK\n"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect "Enter a number that corresponds to your choice > "
send -- "4\r"

expect eof
