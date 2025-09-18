<div style="display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh;">

  <h2>Labs 1-5</h2>
  
  <p>Student ID: 23463452</p>
  <p>Student Name: Rishwanth Katherapalle</p>

</div>

# Lab 1

## AWS Account and Log in

### [1] Log into an IAM user account created for you on AWS.

1. I navigated to the AWS login page:
The login is here: https://489389878001.signin.aws.amazon.com/console, where 489389878001 is the account root user id.

2. I then entered my IAM username: 23463452@student.uwa.edu.au
3. It prompted me to enter the temporary password provided by the teaching team.
4. On first login, I was prompted to **change the password**. Choose a new password and confirm it.  
  
<img width="342" height="399" alt="image" src="https://github.com/user-attachments/assets/77fc7c42-cefc-462c-bbe6-dac7f82f5fd5" />



### [2] Search and open Identity Access Management

1. Click on the user account and click the Security Credentials tab to create access key.
   
   <img width="1920" height="1095" alt="image" src="https://github.com/user-attachments/assets/21679f56-eea3-4761-9e47-486c813fac89" />

3. Make a note of the Access key ID and the secret access key.
   
   <img width="1476" height="106" alt="image" src="https://github.com/user-attachments/assets/388b2bdd-9e8c-4180-a62e-9415c8fd0c46" />


## Set up recent Unix-like OSes

I am already using a WSL2 (Windows Subsystem for Linux) with Ubuntu 22.04 LTS. Installed and using the setup.

<img width="1479" height="757" alt="Screenshot 2025-09-16 092451" src="https://github.com/user-attachments/assets/e10a1690-d307-4e8b-aa41-23e69959b51b" />

## Install Linux packages

### [1] Installing Python

#### Linux and WSL2 Users

1. The recent Linux distributions such as Ubuntu 22.04 LTS, have Python 3.10 installed. I updated the packages to obtain the latest version:

```
sudo apt update
sudo apt -y upgrade
```
<img width="789" height="75" alt="Screenshot 2025-09-16 110132" src="https://github.com/user-attachments/assets/cef9c31f-2bc7-4ff2-ae5e-6dca1a62a1ee" />

2. To check the latest version use:
```
python3 -V
```

3. Next, I installed `pip3`, which is a tool that allows us to install and manage python libraries.

```
sudo apt install -y python3-pip
```

So you can install python packages by: `pip3 install package_name`

<img width="671" height="134" alt="Screenshot 2025-09-16 110207" src="https://github.com/user-attachments/assets/8132e45e-168f-47ed-9535-d645eac064ed" />



### [2] Installing awscli

1. I installed awscli as a wsl2 user using the following commands:
```
sudo apt install awscli
```

Then, 

```
pip3 install awscli --upgrade
```

<img width="590" height="96" alt="Screenshot 2025-09-16 110245" src="https://github.com/user-attachments/assets/1de08278-317d-4e43-9523-0fe062cb0553" />

### [3] Configuring AWS

1. Next, to configure the aws I used the command below:

```
aws configure
```
2. I then saw an output like this:

```
AWS Access Key ID [None]: <Your AWS Access Key ID>
AWS Secret Access Key [None]: <Your AWS Secret Access Key>
Default region name [None]: <Your assigned region>
Default output format [None]: json
```

3. I replaced the placeholder values above with my own AWS Access Key, AWS Secret Access Key and default region name: **ap-northeast-1**.
### [4] Installing boto3

1. Then, I used the following command to install boto3:

```
pip3 install boto3
```

<img width="1081" height="832" alt="Screenshot 2025-09-16 110341" src="https://github.com/user-attachments/assets/43816465-c24f-4aa9-b6f1-c4f198c2bab9" />

## Testing the installed environment

### [1] Testing the AWS environment

1. I used the following command to test the aws environment:

```
aws ec2 describe-regions --output table
```

<img width="996" height="605" alt="Screenshot 2025-09-16 110427" src="https://github.com/user-attachments/assets/7456d790-b202-43ce-a6e1-2940d9901861" />


### [2] Testing the Python environment

1. Now, I used the following script to test the python environment.
   
```
python3
>>> import boto3
>>> ec2 = boto3.client('ec2')
>>> response = ec2.describe_regions()
>>> print(response)
```

2. This created an un-tabulated response.
   <img width="1900" height="628" alt="Screenshot 2025-09-16 110454" src="https://github.com/user-attachments/assets/77a36b84-df08-41e5-af2d-624a569aa992" />


### [3] Write a Python script

1. Now, use the below script to tabulate the un-tabulated response above to have **2 columns** with **Endpoint** and **RegionName**.
   ```
   >>> import pandas as pd
   >>> print(pd.DataFrame(response['Regions'])[['Endpoint', 'RegionName']].to_string(index=False))
   ```
2. It takes the list of regions from response['Regions'], converts it into a Pandas table, keeps only the Endpoint and RegionName columns, formats that table as plain text without row numbers, and prints it.
 <img width="1153" height="553" alt="Screenshot 2025-09-16 111405" src="https://github.com/user-attachments/assets/3c13c892-0b5f-4253-9ca5-29a6756b9051" />


<div style="page-break-after: always;"></div>

# Lab 2

<div style="page-break-after: always;"></div>

# Lab 3

<div style="page-break-after: always;"></div>

# Lab 4

<div style="page-break-after: always;"></div>

# Lab 5
