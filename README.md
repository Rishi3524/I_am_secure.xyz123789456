<div style="display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh;">

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

<img width="1478" height="707" alt="Screenshot 2025-09-16 175713" src="https://github.com/user-attachments/assets/5f0180d1-c262-4904-97ad-047203468cb6" />
<img width="531" height="77" alt="Screenshot 2025-09-16 175724" src="https://github.com/user-attachments/assets/9b642564-aaff-4dc5-ae60-fc4e823b73f6" />


I opened a browser and accessed the address: http://localhost or http://127.0.0.1 to confirm that I got "Hello World!"

<img width="442" height="213" alt="Screenshot 2025-09-16 175521" src="https://github.com/user-attachments/assets/6045ff08-8ee8-4303-bd93-f6a62fec7c2b" />



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

**NOTE**: Delete the created EC2 instance(s) from AWS console after the lab is done.

<div style="page-break-after: always;"></div>

# Lab 3

<div style="page-break-after: always;"></div>

# Lab 4

<div style="page-break-after: always;"></div>
