<img width="1477" height="395" alt="Screenshot 2025-09-18 163810" src="https://github.com/user-attachments/assets/243becc1-4756-498b-95bc-446e8644dd41" /><img width="1474" height="168" alt="Screenshot 2025-09-18 163632" src="https://github.com/user-attachments/assets/4058e292-7e74-42af-b36b-17a72f0fc9e9" />﻿<div style="display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh;">

  <h1>CITS5503 Labs1-5 Report</h1>
  
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


## Creating an EC2 instance using awscli

<img width="1919" height="1134" alt="Screenshot 2025-09-17 104130" src="https://github.com/user-attachments/assets/69843368-d09e-4aa2-9efb-ba96197a522d" />


1. I opened my wsl2 terminal and configured the AWS CLI with my user credentials:
   ```
   aws configure
   ```
   **Region**: **ap-northeast-1**
   
2. I created a new security group to control inbound traffic, it named after my student number with your desired description of it. I used the default description from the labs:

```
aws ec2 create-security-group --group-name 23463452-sg --description "security group for development environment"
```

This will use the default VPC (if you want to specify a VPC, use `--vpc-id vpc-xxxxxxxx`). Take a note of the security group id that is created. 

3. To allow SSH access, I added an inbound (ingress) rule on port 22. An inbound rule permits instances to receive traffic from the specified IPv4 or IPv6 address range:
   
```
aws ec2 authorize-security-group-ingress --group-name 23463452-sg --protocol tcp --port 22 --cidr 0.0.0.0/0
```

3. I generated a key pair for secure SSH login:

```
aws ec2 create-key-pair --key-name 23463452-key --query 'KeyMaterial' --output text > 23463452-key.pem
```

4. To use this key I moved the file to ~/.ssh and changed the permissions to:
   
```
mkdir -p ~/.ssh
mv 23463452-key.pem ~/.ssh/
chmod 400 ~/.ssh/23463452-key.pem
```

5. I launched an EC2 instance (t3.micro) with the provided AMI for my region **ap-northeast-1** and based on the security group id from the above steps:

```
 aws ec2 run-instances --image-id ami-054400ced365b82a0 --security-group-ids sg-04f5a7ee9c2753385 23463452-sg --count 1 --instance-type t3.micro --key-name 23463452-key --query 'Instances[0].InstanceId'

 ```

6. I tagged the instance with a proper name (23463452-vm) and based on the instance from the above step:
   
 ```
  aws ec2 create-tags --resources i-0e0ff961c57ddc589 --tags Key=Name,Value=23463452-vm
 ```
**NOTE**: If you create a single instance, you must name it using the format of `<student number>-vm` (e.g., 24242424-vm). If you need to create multiple ones, follow the naming format of `<student number>-vm1` and `<student number>-vm2` (e.g., 24242424-vm1, 24242424-vm2).

7. I retrieved the public IP for SSH connection based on the instance id from above:
   
```
aws ec2 describe-instances --instance-ids i-0e0ff961c57ddc589 --query 'Reservations[0].Instances[0].PublicIpAddress'
```

8. I connected via SSH: based on the IP address from above step:

```
ssh -i 23463452-key.pem ubuntu@35.77.17.205
```

Now, go to the AWS Console and search the ec2 instances where we get our created ec2  instance.

<img width="1422" height="465" alt="image" src="https://github.com/user-attachments/assets/c8c5dda3-06c5-409a-b11e-73ec7e251181" />




## Create an EC2 instance with Python Boto3

For this task I used the following Python script named **create_ec2.py** to implement the steps above for creating an EC2 instance using Boto3 based on steps 1 to 6: 
1. Copy the following script.
   
```
from __future__ import annotations

import os
from pathlib import Path

import boto3
from botocore.exceptions import ClientError, NoCredentialsError

STUDENT_NUMBER = "23463452"
REGION = "ap-northeast-1"
AMI_ID = "ami-054400ced365b82a0"
INSTANCE_TYPE = "t3.micro"


def _default_vpc_id(ec2) -> str: # Basically to get the vpc id for creating the security group using the functions.
    vpcs = ec2.describe_vpcs(Filters=[{"Name": "isDefault", "Values": ["true"]}]).get("Vpcs", [])
    if not vpcs:
        raise RuntimeError("No default VPC in region.")
    return vpcs[0]["VpcId"]


def _default_subnet_id(ec2, vpc_id: str) -> str: # Needed to run the instance of EC2.
    subnets = ec2.describe_subnets(
        Filters=[{"Name": "vpc-id", "Values": [vpc_id]}, {"Name": "default-for-az", "Values": ["true"]}]
    ).get("Subnets", [])
    if not subnets:
        subnets = ec2.describe_subnets(Filters=[{"Name": "vpc-id", "Values": [vpc_id]}]).get("Subnets", [])
        if not subnets:
            raise RuntimeError("No subnet in default VPC.")
    return sorted(subnets, key=lambda s: s["AvailabilityZone"])[0]["SubnetId"]


def main() -> int:
    sg_name = f"{STUDENT_NUMBER}-sg"
    key_name = f"{STUDENT_NUMBER}-key"
    inst_name = f"{STUDENT_NUMBER}-vm"
    pem_path = Path.home() / ".ssh" / f"{key_name}.pem"

    try:
        ec2 = boto3.client("ec2", region_name=REGION)

        # 1) Creates a Security Group
        vpc_id = _default_vpc_id(ec2)
        sg_resp = ec2.create_security_group(
            Description="security group for development environment",
            GroupName=sg_name,
            VpcId=vpc_id,
        )
        sg_id = sg_resp["GroupId"]
        # 2) Allows SSH access and permit instance to receive traffic from port 22
        ec2.authorize_security_group_ingress(
            GroupId=sg_id,
            IpPermissions=[
                {
                    "IpProtocol": "tcp",
                    "FromPort": 22,
                    "ToPort": 22,
                    "IpRanges": [{"CidrIp": "0.0.0.0/0", "Description": "SSH"}],
                }
            ],
        )

        # 3) Create Key Pair and write the permissions
        pem_path.parent.mkdir(parents=True, exist_ok=True)
        key_resp = ec2.create_key_pair(KeyName=key_name)
        pem_path.write_text(key_resp["KeyMaterial"], encoding="utf-8")
        os.chmod(pem_path, 0o400)

        # 4) Run the instance
        subnet_id = _default_subnet_id(ec2, vpc_id)
        run_resp = ec2.run_instances(
            ImageId=AMI_ID,
            InstanceType=INSTANCE_TYPE,
            MinCount=1,
            MaxCount=1,
            KeyName=key_name,
            NetworkInterfaces=[
                {
                    "DeviceIndex": 0,
                    "SubnetId": subnet_id,
                    "AssociatePublicIpAddress": True,
                    "Groups": [sg_id],
                }
            ],
        )
        instance_id = run_resp["Instances"][0]["InstanceId"]

        # 5) Tag the instance using a Name 
        ec2.create_tags(Resources=[instance_id], Tags=[{"Key": "Name", "Value": inst_name}])

        # 6) get Public IP
        ec2.get_waiter("instance_running").wait(InstanceIds=[instance_id])
        described = ec2.describe_instances(InstanceIds=[instance_id])
        public_ip = described["Reservations"][0]["Instances"][0].get("PublicIpAddress", "")
        if public_ip:
            print(f"PublicIp: {public_ip}")
            print(f"SSH: ssh -i '{pem_path}' ubuntu@{public_ip}")
        return 0

    except NoCredentialsError:
        print("ERROR: Configure AWS credentials (e.g., `aws configure`).")
    except ClientError as e:
        print(f"AWS ERROR: {e.response.get('Error', {}).get('Code', 'ClientError')}: {e}")
    except Exception as e:
        print(f"ERROR: {e}")

    return 1


if __name__ == "__main__":
    raise SystemExit(main())

```
**The breakdown of the functions used in the script**:

**_default_vpc_id(ec2) -> str**:

Input: takes a Boto3 EC2 client.
Action: It filters describe_vpcs by isDefault=true; chooses the first match.
Output: The default VPC ID (e.g., vpc-abc123) is given.

**_default_subnet_id(ec2, vpc_id: str) -> str**:

Input: It takes a EC2 client and a VPC ID.
Action: It tries describe_subnets with the filters vpc-id=<vpc_id> and default-for-az=true (auto public IP). If none of them then, fall back to any subnet in the VPC. It is then sorted by AvailabilityZone and picks the first one to determine.
Output: a Subnet ID is in which the EC2 instance lives.

**main() -> int**:

This implements all the steps of AWS CLI preparing the names/paths: builds security group name, key name, instance name, and PEM path under ~/.ssh/.

EC2 client: boto3.client("ec2", region_name=REGION).

1) sg_resp = ec2.create_security_group(): creates SG in the default VPC with description; saves sg_id.

2) ec2.authorize_security_group_ingress(): opens TCP/22 from 0.0.0.0/0 (world-open for convenience; risky on the open internet).

3) key_resp = ec2.create_key_pair(): creates key pair, writes PEM to disk, sets 0400 permissions.

4) run_resp = ec2.run_instances(): chooses a subnet from the above function, runs one t3.micro from the given AMI; attaches SG; ensures public IP via AssociatePublicIpAddress=True.

5) ec2.create_tags(): sets Name=23463452-vm.

6) desc = ec2.describe_instances():describes the instance.

Returns: 0 on success, 1 on handled errors.

**Note**: Refer to [page](https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/ec2.html) for details of all thefunctions used to initiate the EC2instance. 

2. Now run this script using:

```
python3 create_ec2.py
```

Another instance is created after the first one made using AWS CLI:

<img width="1919" height="491" alt="Screenshot 2025-09-17 104418" src="https://github.com/user-attachments/assets/bc157c7c-7b31-4bb8-92ac-c8984f0ad8c4" />


## Install Docker


1. I installed Docker on my WSL2 environment and started and enabled it. 
```
sudo apt install docker.io -y
sudo systemctl start docker
sudo systemctl enable docker
docker --version     # To check the version
```
<img width="861" height="302" alt="Screenshot 2025-09-16 175936" src="https://github.com/user-attachments/assets/a6a1a91b-2f3f-4179-aec0-906ecffff839" />

## Building and running an httpd container

2. I created a directory called html using the following commands:

```
mkdir html
```

3. I edited a file index.html inside the html directory:
   
```
nano html/index.html
```
4. Then added the following content in the nano editor:

```
  <html>
    <head> </head>
    <body>
      <p>Hello World!</p>
    </body>
  </html>
```
Then exit using Ctrl+O, ENTER, Ctrl+X to save the script and exit.
<img width="1181" height="296" alt="Screenshot 2025-09-16 180014" src="https://github.com/user-attachments/assets/9fc28c33-e3ff-4ca2-a236-6d020b545aff" />


5. Then I created a file called Dockerfile outside the html directory with the following content:
   <img width="863" height="75" alt="Screenshot 2025-09-16 180102" src="https://github.com/user-attachments/assets/57ea12a0-4684-44b9-bb44-dbd668c07e27" />
```
nano Dockerfile
```
Put this script there:
```
FROM httpd:2.4
COPY ./html/ /usr/local/apache2/htdocs/
```
Exit as above.

<img width="866" height="112" alt="Screenshot 2025-09-16 180133" src="https://github.com/user-attachments/assets/5a7e321e-9177-4e68-b368-984c9c911225" />



6. Now I built a docker image using the following command.

```
docker build -t my-apache2 .
```

If you run into permission errors, you may need add your user to the docker group:

```
sudo usermod -a -G docker <username>
```

Make sure to log out and log back in for this change to take effect.

7. Now I run the image:

```
docker run -p 80:80 -dit --name my-app my-apache2
```

8. I used this command to list all Docker containers, including both running and stopped ones:

```
docker ps -a
```

9. I stopped the running container:

```
docker stop my-app
```

10. Then, I removed the stopped container:
```
docker rm my-app
```


<img width="1478" height="707" alt="Screenshot 2025-09-16 175713" src="https://github.com/user-attachments/assets/5f0180d1-c262-4904-97ad-047203468cb6" />
<img width="531" height="77" alt="Screenshot 2025-09-16 175724" src="https://github.com/user-attachments/assets/9b642564-aaff-4dc5-ae60-fc4e823b73f6" />


I opened a browser and accessed the address: http://localhost or http://127.0.0.1 to confirm that I got "Hello World!"

<img width="442" height="213" alt="Screenshot 2025-09-16 175521" src="https://github.com/user-attachments/assets/6045ff08-8ee8-4303-bd93-f6a62fec7c2b" />


**NOTE**: Delete the created EC2 instance(s) from AWS console after the lab is done.

<div style="page-break-after: always;"></div>

# Lab 3

## Program

### Preparation

Download the python code `cloudstorage.py` from the directory of [src](https://github.com/zhangzhics/CITS5503_Sem2/blob/master/Labs/src/cloudstorage.py) \

1. I ran the following commands on WSL2 to setup the required files and directories along with the content in it:

```
mkdir rootdir
echo -e "1\n2\n3\n4\n5\n" > rootdir/rootfile.txt

mkdir rootdir/subdir
echo -e "1\n2\n3\n4\n5\n" > rootdir/subdir/subfile.txt

```

We should now have:

rootdir/
 ├── rootfile.txt
 └── subdir/
     └── subfile.txt

<img width="1175" height="90" alt="Screenshot 2025-09-17 194538" src="https://github.com/user-attachments/assets/b8bcb863-3544-45fb-9130-9632b08e4ab1" />


### Saving to S3 using `cloudstorage.py`

1. I modified and used the following Python script, `cloudstorage.py`:
   
```
import os
import boto3
import botocore

ROOT_DIR = 'rootdir'
STUDENT_ID = '23463452' 
ROOT_S3_DIR = f"{STUDENT_ID}-cloudstorage"

REGION = "ap-northeast-1" 
s3 = boto3.client("s3")

try:
    if REGION == "us-east-1":
        s3.create_bucket(Bucket=ROOT_S3_DIR) # for default region
    else:
        s3.create_bucket(
            Bucket=ROOT_S3_DIR,
            CreateBucketConfiguration={'LocationConstraint': REGION} # for a specific region
        )
except botocore.exceptions.ClientError as e:
    if e.response['Error']['Code'] == 'BucketAlreadyOwnedByYou':
        print(f"Bucket '{ROOT_S3_DIR}' already exists.")
    else:
        raise e

def upload_file(folder_name, file, file_name): # function to upload a file
    key = os.path.join(folder_name, file_name) if folder_name else file_name
    print(f"Uploading {file} to s3://{ROOT_S3_DIR}/{key}")
    s3.upload_file(file, ROOT_S3_DIR, key)

for dir_name, subdir_list, file_list in os.walk(ROOT_DIR, topdown=True): # uploads each file onto the S3 bucket
    relative_dir = os.path.relpath(dir_name, ROOT_DIR)
    relative_dir = "" if relative_dir == "." else relative_dir
    for fname in file_list:
        file_path = os.path.join(dir_name, fname)
        upload_file(relative_dir, file_path, fname)




```

2. This code creates an S3 bucket named `23463452-cloudstorage`. This program traverses the directory starting at the root directory `rootdir`, then it uploads each file onto the S3 bucket using the command **s3.upload_file()**.
3. I made sure that my S3 bucket has the same file structure as shown in `Preparation` above.
4. It takes my region "ap-northeast-1" as the location constrain.


### Restoring from S3

1. I created a new program called `restorefromcloud.py` that reads the S3 bucket and writes the contents of the bucket within the appropriate directories as follows:

```
import os
import boto3

STUDENT_ID = '23463452' 
ROOT_S3_DIR = f"{STUDENT_ID}-cloudstorage"
RESTORE_DIR = "restored_rootdir"

s3 = boto3.client("s3")


objects = s3.list_objects_v2(Bucket=ROOT_S3_DIR)

if 'Contents' not in objects:
    exit(0)

for obj in objects['Contents']:
    key = obj['Key']
    local_path = os.path.join(RESTORE_DIR, key)
    os.makedirs(os.path.dirname(local_path), exist_ok=True)
    s3.download_file(ROOT_S3_DIR, key, local_path)

```
 
2. Now use **aws configure** with your credentials as stated in lab2:
```
aws configure
```

3.. Now I run the above two scripts in succession:
   
```
python3 cloudstorage.py
python3 restorefromcloud.py

```

<img width="1075" height="280" alt="Screenshot 2025-09-17 194553" src="https://github.com/user-attachments/assets/0d2b7526-2ae1-4137-82d7-2d3343c6525e" />

Now the S3 bucket is made on AWS with the required files.

<img width="1919" height="551" alt="Screenshot 2025-09-17 195124" src="https://github.com/user-attachments/assets/7c592278-1add-45c7-83ef-168160b6b9bc" />
<img width="1919" height="617" alt="Screenshot 2025-09-17 195141" src="https://github.com/user-attachments/assets/e92eada3-97d9-4d23-9725-74d6de3893bc" />

My local environment see a copy of the files and the directories from the S3 bucket.
<img width="1016" height="848" alt="image" src="https://github.com/user-attachments/assets/9fe591df-0976-4539-8217-a4dd57ce3b2d" />



### Writing information about files to DynamoDB

1. I installed DynamoDB on my WSL2 environment using the following commands:

```
mkdir dynamodb
cd dynamodb
```

2. I installed jre (if not done): 

```
sudo apt-get install default-jre
wget https://s3-ap-northeast-1.amazonaws.com/dynamodb-local-tokyo/dynamodb_local_latest.tar.gz
```

<img width="1157" height="163" alt="Screenshot 2025-09-17 194610" src="https://github.com/user-attachments/assets/7dc4a711-b613-405a-9efa-7fd1365db573" />



3. Now I can use the following command to extract files from dynamodb_local_latest.tar.gz

```
tar -zxvf dynamodb_local_latest.tar.gz
```

<img width="1897" height="307" alt="Screenshot 2025-09-17 194637" src="https://github.com/user-attachments/assets/c072f63e-71d6-4ab6-ba5a-e37ba071e771" />


After the extraction, I run the command below:

```
java -Djava.library.path=./DynamoDBLocal_lib -jar DynamoDBLocal.jar -sharedDb
```

Alternatively, you can use docker:
```
docker run -p 8000:8000 amazon/dynamodb-local -jar DynamoDBLocal.jar -inMemory -sharedDb
```
**Note**: Do not close the current window, open a new window to run the following Python script.

<img width="1919" height="696" alt="Screenshot 2025-09-17 194858" src="https://github.com/user-attachments/assets/60a7fd8e-9eb6-4f44-a4ae-b80f9249a2af" />


4. Now I made a Python script **write_cloudfiles.py** to create a table called `CloudFiles` on my local DynamoDB as follows:
   
```
import os
import boto3
from botocore.exceptions import ClientError

STUDENT_ID = "23463452"                 
BUCKET = f"{STUDENT_ID}-cloudstorage"
REGION = "ap-northeast-1"               
DDB_ENDPOINT = "http://localhost:8000"  # DynamoDB Local

WANTED = {"rootfile.txt", "subdir/subfile.txt"}

# Create and connect to localDynamoDB

dynamodb = boto3.resource("dynamodb", region_name=REGION, endpoint_url=DDB_ENDPOINT)
s3 = boto3.client("s3", region_name=REGION)

# Create table if not exists
try:
    dynamodb.create_table(
        TableName="CloudFiles",
        KeySchema=[
            {"AttributeName": "userId", "KeyType": "HASH"},
            {"AttributeName": "fileName", "KeyType": "RANGE"},
        ],
        AttributeDefinitions=[
            {"AttributeName": "userId", "AttributeType": "S"},
            {"AttributeName": "fileName", "AttributeType": "S"},
        ],
        ProvisionedThroughput={"ReadCapacityUnits": 5, "WriteCapacityUnits": 5},
    )
except ClientError as e:
    if e.response["Error"]["Code"] != "ResourceInUseException":
        raise

table = dynamodb.Table("CloudFiles")

# Pull objects from S3
resp = s3.list_objects_v2(Bucket=BUCKET)
if "Contents" not in resp:
    print("No objects in bucket.")
    raise SystemExit(0)

inserted = 0
for obj in resp["Contents"]:
    key = obj["Key"]
    if key not in WANTED:
        continue

    file_name = os.path.basename(key)
    path = os.path.dirname(key)
    
# Fetch ACL to fill userId, owner, permissions
    acl = s3.get_object_acl(Bucket=BUCKET, Key=key)
    owner_block = acl.get("Owner", {})
    owner = owner_block.get("DisplayName") or owner_block.get("ID", "")
    user_id = owner_block.get("ID", "")
    perms = [g.get("Permission") for g in acl.get("Grants", []) if g.get("Permission")]

    table.put_item(Item={
        "userId": user_id,
        "fileName": file_name,
        "path": path,
        "lastUpdated": obj["LastModified"].isoformat(),
        "owner": owner,
        "permissions": ",".join(perms),
    })

    inserted += 1
    if inserted == 2: # Stops after inserting 2 items (inserted == 2) to match the assignment requirement.
        break



```




5. This code make the table CloudFiles whose attributes are:

```
        CloudFiles = {
            'userId',
            'fileName',
            'path',
            'lastUpdated',
	    	'owner',
            'permissions'
            }
```
Here `userId` is the partition key and `fileName` is the sort key. Regarding the creation.

**Functions used**:
1) s3.list_objects_v2(): Used to fetch/list objects from the bucket.
2) s3.get_object_acl(): Retrieves the access control list (ACL) for a specific object.

6. Then, I got the attributes above for each file of the S3 bucket and then wrote the attributes of each file into the created DynamoDB table.

7.  This code gives a table which will have 2 items. One item corresponds to one file in the bucket and consists of the attributes above and their values.
 
For the functions used:  
Refer to this [page](https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/dynamodb.html)
Refer to this [page](https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/s3/client/get_object_acl.html)
Refer to this [page](https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/dynamodb.html)

8. Now run the script using:

```
python3 run_cloudfiles.py
```

### Scan the table

1. Now I used the AWS CLI command to scan the created DynamoDB table, and output what you've got using the following command:
```
aws dynamodb scan --table-name CloudFiles --endpoint-url http://localhost:8000

```
<img width="949" height="198" alt="Screenshot 2025-09-17 194858" src="https://github.com/user-attachments/assets/38d3a73c-9463-48d8-a31f-e592fdcde469" />

<img width="1133" height="1091" alt="Screenshot 2025-09-17 194922" src="https://github.com/user-attachments/assets/a87d633a-f4fc-4f25-a135-bf1dd8bf4d51" />


### [6] Delete the table

1. Now I use the AWS CLI command to delete the table:

```
aws dynamodb delete-table --table-name CloudFiles --endpoint-url http://localhost:8000

```
<img width="1709" height="315" alt="Screenshot 2025-09-17 194949" src="https://github.com/user-attachments/assets/973a680a-1918-4577-a8df-8ebd422fe612" />
<img width="1422" height="115" alt="Screenshot 2025-09-17 195001" src="https://github.com/user-attachments/assets/57d77766-c6ba-403e-904d-aafc9ea4f698" />


**NOTE**: Delete the created S3 bucket(s) from AWS console after the lab is done.


<div style="page-break-after: always;"></div>

# Lab 4

## Apply a policy to restrict permissions on the bucket

### Write a Python script

1. I made the following python script **apply_bucket_policy.py** to apply the a policy to an S3 bucket to allow only your username to access the bucket. 


```
import boto3
import json

bucket_name = "23463452-cloudstorage"
student_number = "23463452"  
username = f"{student_number}@student.uwa.edu.au"

# bucket policy
bucket_policy = {
    "Version": "2012-10-17",
    "Statement": [
        {
            "Sid": "RestrictAccessToUserOnly",
            "Effect": "Deny",
            "Principal": "*",
            "Action": "s3:*",
            "Resource": [
                f"arn:aws:s3:::{bucket_name}",         # bucket itself
                f"arn:aws:s3:::{bucket_name}/*"       # all objects inside
            ],
            "Condition": {
                "StringNotLike": {
                    "aws:username": username
                }
            }
        }
    ]
}

# Convert dict to JSON
bucket_policy_json = json.dumps(bucket_policy)

s3 = boto3.client('s3')

# Apply policy
try:
    s3.put_bucket_policy(Bucket=bucket_name, Policy=bucket_policy_json)
    print(f"Policy successfully applied to: {bucket_name}")
except Exception as e:
    print("Error in applying the policy:", e)


```

Uses **s3.put_bucket_policy()** to apply the given policy in lab4 to an S3 bucket to allow only your username to access the bucket.
2. Configure AWS just as mention in the previous labs and run the script:

```
python3 apply_bucket_policy.py
```

### Check whether the script works

1. I used the AWS CLI command and the AWS S3 console to see if it displays the policy content applied to the S3 bucket.

   **AWS CLI command**:
   ```
   aws s3api get-bucket-policy --bucket <your_s3_bucket> --query Policy --output text

   ```
   <img width="1474" height="168" alt="Screenshot 2025-09-18 163632" src="https://github.com/user-attachments/assets/bf574832-020e-4dbd-ae4a-70ccda5d3d80" />

**AWS S3 Console**:
<img width="1919" height="940" alt="Screenshot 2025-09-18 163719" src="https://github.com/user-attachments/assets/7451d483-ff19-46be-aeee-6a6b842f6fc5" />



2. Now I tested the policy by using a username that is not mine to access the folder called `rootdir` and output what I got.
   Now I created two profiles to access the s3 bucket from different accounts:
  ```
  aws configure --profile 23463452
  aws configure --profile 23237076 
  ```
Now 23463452 is in 23463452@student.uwa.edu.au username and 23237076 is in Rishi352004 username and now I use the following command to access to the bucket to which the policy was applied:

```
  aws s3 ls s3://<your_s3_bucket>/rootdir/ --profile 23463452
  aws s3 ls s3://<your_s3_bucket>/rootdir/ --profile 23237076
```

Then it denies access to the user 23237076.

<img width="1477" height="395" alt="Screenshot 2025-09-18 163810" src="https://github.com/user-attachments/assets/4c82749b-9781-4897-b493-7808f0263ddc" />


## AES Encryption using KMS

### Create a KMS key

1. I made a Python script **create_kms_key.py** to create a KMS key, where my student number works as an alias for the key.

### Attach a policy to the created KMS key

1. I updated the above script to attach the following policy to the key.



```
{
  "Version": "2012-10-17",
  "Id": "key-consolepolicy-3",
  "Statement": [
    {
      "Sid": "Enable IAM User Permissions",
      "Effect": "Allow",
      "Principal": {
        "AWS": "arn:aws:iam::489389878001:root"
      },
      "Action": "kms:*",
      "Resource": "*"
    },
    {
      "Sid": "Allow access for Key Administrators",
      "Effect": "Allow",
      "Principal": {
        "AWS": "arn:aws:iam::489389878001:user/<your_username>"
      },
      "Action": [
        "kms:Create*",
        "kms:Describe*",
        "kms:Enable*",
        "kms:List*",
        "kms:Put*",
        "kms:Update*",
        "kms:Revoke*",
        "kms:Disable*",
        "kms:Get*",
        "kms:Delete*",
        "kms:TagResource",
        "kms:UntagResource",
        "kms:ScheduleKeyDeletion",
        "kms:CancelKeyDeletion"
      ],
      "Resource": "*"
    },
    {
      "Sid": "Allow use of the key",
      "Effect": "Allow",
      "Principal": {
        "AWS": "arn:aws:iam::489389878001:user/<your_username>"
      },
      "Action": [
        "kms:Encrypt",
        "kms:Decrypt",
        "kms:ReEncrypt*",
        "kms:GenerateDataKey*",
        "kms:DescribeKey"
      ],
      "Resource": "*"
    },
    {
      "Sid": "Allow attachment of persistent resources",
      "Effect": "Allow",
      "Principal": {
        "AWS": "arn:aws:iam::489389878001:user/<your_username>"
      },
      "Action": [
        "kms:CreateGrant",
        "kms:ListGrants",
        "kms:RevokeGrant"
      ],
      "Resource": "*",
      "Condition": {
        "Bool": {
          "kms:GrantIsForAWSResource": "true"
        }
      }
    }
  ]
}
```

2. Now the code is:
```
import boto3
import json

student_number = "23463452"
username = "23463452@student.uwa.edu.au" 

kms_client = boto3.client("kms")

# 1. Create the KMS key
response = kms_client.create_key(
    Description=f"KMS key for {username}",
    KeyUsage="ENCRYPT_DECRYPT",
    CustomerMasterKeySpec="SYMMETRIC_DEFAULT",
    Origin="AWS_KMS"
)

key_id = response["KeyMetadata"]["KeyId"]

# 2. Create an alias for the key using student number
alias = f"alias/{student_number}"
kms_client.create_alias(
    AliasName=alias,
    TargetKeyId=key_id
)



key_policy = {
    "Version": "2012-10-17",
    "Id": "key-consolepolicy-3",
    "Statement": [
        {
            "Sid": "Enable IAM User Permissions",
            "Effect": "Allow",
            "Principal": {
                "AWS": "arn:aws:iam::489389878001:root"
            },
            "Action": "kms:*",
            "Resource": "*"
        },
        {
            "Sid": "Allow access for Key Administrators",
            "Effect": "Allow",
            "Principal": {
                "AWS": f"arn:aws:iam::489389878001:user/{username}"
            },
            "Action": [
                "kms:Create*","kms:Describe*","kms:Enable*","kms:List*",
                "kms:Put*","kms:Update*","kms:Revoke*","kms:Disable*",
                "kms:Get*","kms:Delete*","kms:TagResource","kms:UntagResource",
                "kms:ScheduleKeyDeletion","kms:CancelKeyDeletion"
            ],
            "Resource": "*"
        },
        {
            "Sid": "Allow use of the key",
            "Effect": "Allow",
            "Principal": {
                "AWS": f"arn:aws:iam::489389878001:user/{username}"
            },
            "Action": [
                "kms:Encrypt","kms:Decrypt","kms:ReEncrypt*",
                "kms:GenerateDataKey*","kms:DescribeKey"
            ],
            "Resource": "*"
        },
        {
            "Sid": "Allow attachment of persistent resources",
            "Effect": "Allow",
            "Principal": {
                "AWS": f"arn:aws:iam::489389878001:user/{username}"
            },
            "Action": [
                "kms:CreateGrant","kms:ListGrants","kms:RevokeGrant"
            ],
            "Resource": "*",
            "Condition": {
                "Bool": {"kms:GrantIsForAWSResource": "true"}
            }
        }
    ]
}

kms_client.put_key_policy(
    KeyId=key_id,
    Policy=json.dumps(key_policy),
    PolicyName="default"
)




```

Run the script using **python3 create_kms_key.py**.

<img width="1919" height="455" alt="Screenshot 2025-09-18 175444" src="https://github.com/user-attachments/assets/906f9a20-a102-4c7f-b64e-b2018e77856e" />


### Check whether the script works

1. I used the AWS KMS console to test whether your username is the key administrator and key user.


<img width="1483" height="497" alt="Screenshot 2025-09-18 175518" src="https://github.com/user-attachments/assets/f70d21e5-c241-4861-ac8d-172979e1b8cb" />
<img width="1470" height="487" alt="Screenshot 2025-09-18 175536" src="https://github.com/user-attachments/assets/b24114d8-8f6a-498a-9596-b212f23ad283" />
<img width="1473" height="473" alt="Screenshot 2025-09-18 175549" src="https://github.com/user-attachments/assets/8daa8e56-72e9-41d8-9433-1fa1fc8d9e2a" />

### Use the created KMS key for encryption/decryption

1. I made a Python script **kms_encrypt_decrypt.py** where each file from the S3 bucket is encrypted and then decrypted via the created KMS key. Both encrypted and decrypted files will be in the same folder as the original file.
   
```
import boto3
import os

# Your KMS alias and bucket
kms_key_id = "alias/23463452"
bucket_name = "23463452-cloudstorage"

s3 = boto3.client("s3")
kms = boto3.client("kms")

def kms_encrypt_file(filename):
    with open(filename, "rb") as f:
        plaintext = f.read()

    response = kms.encrypt(KeyId=kms_key_id, Plaintext=plaintext)
    ciphertext = response["CiphertextBlob"]

    enc_filename = filename + ".kms.enc"
    with open(enc_filename, "wb") as f:
        f.write(ciphertext)

    return enc_filename

def kms_decrypt_file(enc_filename):
    with open(enc_filename, "rb") as f:
        ciphertext = f.read()

    response = kms.decrypt(CiphertextBlob=ciphertext)
    plaintext = response["Plaintext"]

    dec_filename = enc_filename.replace(".kms.enc", ".kms.dec")
    with open(dec_filename, "wb") as f:
        f.write(plaintext)

    return dec_filename

def upload_to_s3(local_file):
    key = os.path.basename(local_file)
    s3.upload_file(local_file, bucket_name, key)
    print(f"Uploaded {local_file} → s3://{bucket_name}/{key}")


objects = s3.list_objects_v2(Bucket=bucket_name)

if "Contents" in objects:
    for obj in objects["Contents"]:
        key = obj["Key"]
        filename = os.path.basename(key)

        # Skip already encrypted/decrypted files
        if filename.endswith((".kms.enc", ".kms.dec")):
            continue

        # Download file
        s3.download_file(bucket_name, key, filename)

        # Encrypt + upload
        enc_file = kms_encrypt_file(filename)
        upload_to_s3(enc_file)

        # Decrypt + upload
        dec_file = kms_decrypt_file(enc_file)
        upload_to_s3(dec_file)



```

Run the script using **python3 kms_encrypt_decrypt.py**.


### Apply `pycryptodome` for encryption/decryption

1. I made another Python script **pycrypto_encrypt_decrypt.py** that uses the Python library `pycryptodome` to encrypt and decrypt each file in the S3 bucket. Both encrypted and decrypted files will be in the same folder as the original file.

For encryption/decryption, refer to the example code from [fileencrypt.py](https://github.com/zhangzhics/CITS5503_Sem2/blob/master/Labs/src/fileencrypt.py).

```
  import os
import boto3
import hashlib, struct
from Crypto.Cipher import AES
from Crypto import Random

bucket_name = "23463452-cloudstorage"
password = "kitty and the kat"

s3 = boto3.client("s3")

BLOCK_SIZE = 16
CHUNK_SIZE = 64 * 1024

def encrypt_file(password, in_filename, out_filename):
    key = hashlib.sha256(password.encode("utf-8")).digest()
    iv = Random.new().read(AES.block_size)
    encryptor = AES.new(key, AES.MODE_CBC, iv)
    filesize = os.path.getsize(in_filename)

    with open(in_filename, "rb") as infile:
        with open(out_filename, "wb") as outfile:
            outfile.write(struct.pack("<Q", filesize))
            outfile.write(iv)
            while True:
                chunk = infile.read(CHUNK_SIZE)
                if len(chunk) == 0:
                    break
                elif len(chunk) % 16 != 0:
                    chunk += b" " * (16 - len(chunk) % 16)
                outfile.write(encryptor.encrypt(chunk))

def decrypt_file(password, in_filename, out_filename):
    key = hashlib.sha256(password.encode("utf-8")).digest()
    with open(in_filename, "rb") as infile:
        origsize = struct.unpack("<Q", infile.read(struct.calcsize("Q")))[0]
        iv = infile.read(16)
        decryptor = AES.new(key, AES.MODE_CBC, iv)
        with open(out_filename, "wb") as outfile:
            while True:
                chunk = infile.read(CHUNK_SIZE)
                if len(chunk) == 0:
                    break
                outfile.write(decryptor.decrypt(chunk))
            outfile.truncate(origsize)

def upload_to_s3(local_file):
    key = os.path.basename(local_file)
    s3.upload_file(local_file, bucket_name, key)
    print(f"Uploaded {local_file} → s3://{bucket_name}/{key}")


objects = s3.list_objects_v2(Bucket=bucket_name)

if "Contents" in objects:
    for obj in objects["Contents"]:
        key = obj["Key"]
        filename = os.path.basename(key)

        # Skip already encrypted/decrypted files
        if filename.endswith((".pycrypto.enc", ".pycrypto.dec")):
            continue

        # Download file
        s3.download_file(bucket_name, key, filename)

        # Encrypt + upload
        enc_file = filename + ".pycrypto.enc"
        encrypt_file(password, filename, enc_file)
        upload_to_s3(enc_file)

        # Decrypt + upload
        dec_file = filename + ".pycrypto.dec"
        decrypt_file(password, enc_file, dec_file)
        upload_to_s3(dec_file)



```
Run the script using **pycrypto_encrypt_decrypt.py**.



### Uploading

1. I uploaded all encrypted and decrypted files to your S3 bucket from the above codes.

<img width="1891" height="715" alt="image" src="https://github.com/user-attachments/assets/dc1cb855-6802-40fc-8925-355407be0ec8" />


**NOTE**: Delete the created S3 bucket(s) and KMS key(s) from AWS console after the lab is done.



<div style="page-break-after: always;"></div>
