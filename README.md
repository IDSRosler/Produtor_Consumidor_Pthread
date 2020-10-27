# Producer_Consumer_Pthread

## Build
* In src_files folder run the command:
    * g++ main.cpp -lpthread -o <exe_name>
    
## Run
* Run the command:
    * ./<exe_name> -pt < integer_value > -ct < integer_value > -iq < integer_value > -bs < integer_value >
    
## Parameters (CLI)
| Parameters                  | Type      | Value    | Description |
|-----------------------------|:---------:|:--------:|-------------|
| -pt                         | Int       | Required | Producer threads |
| -ct                         | Int       | Required | Consumer threads |
| -iq                         | Int       | Required | Items quantity   |
| -bs                         | Int       | Required | Buffer size      |