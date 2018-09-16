# ProtocolGuy

## Main Menu commands

- I2C
- SPI
- help

## I2C Commands

- addr=[slave address]

        Command to set the current I2C slave address.
        Ex: To set address 0x23

        'addr=0x23'

- w=[data]

        Command to write data on the bus at the configured address
        Ex: To send 3 bytes [0,1,2] :

        'w=0x00 0x01 0x02'
        'w=0X00,0X01,0X02'
        'w=0 1 2'
        'w=0,1,2'
        'w=0.1.2'

- wr=[register]

        Command to read a specific register at the configured slave address
        Ex: To read register 0x00 :

        'wr=0x00'
        'wr=0'

- r

        Send an I2C read command to the configured register
        'r=' ** Not implemented yet **

- h

- scan

        Scan the available devices on the bus

## SPI Commands

- w=[data]

- wr

- r
- h
