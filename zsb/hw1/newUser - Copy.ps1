# VARIABLES
# Change the variables according to usage and password policies
# Policies enforced in this script: 1] minimal length 2] check for common passwords
$MINIMAL_LENGTH = 12; # Define the minimal length of the password
$PATH = "C:\Users\Dolan\ZSB\table.txt" # Define the path to table with weak passwords for checks
$GROUP = "Users" # Define the group of the new user



# Get name
$name = Read-Host "Enter username"

# Get password
Write-Output "Password should adhere to following policies:" "1) Minimum of $MINIMAL_LENGTH characters" "2) Be careful about using trivial passwords" "3) Avoid using typical structure of passwords: capital letters at the beginning, numbers and special characters at the end, ..."
$password

# Check the password according to enforced policies
do {
    # Get password from user
    $password = Read-Host "Enter Password" -AsSecureString

    # convert to plain text for testing purposes
#    $password = ConvertFrom-SecureString $password -AsPlainText # !!! -AsPlainText available from PowerShell version 7. !!!
    $wrongPassword = 1;

    # Check the required length of the password
    if ("$password".Length -lt $MINIMAL_LENGTH)
    {
        Write-Output "Password is too short. Use at least $MINIMAL_LENGTH characters."
        continue
    }

    # Consult table of commonly-used passwords if is is a weak password
    try {
        if (Select-String -Path "$PATH" -Pattern "$password" -SimpleMatch -Quiet)
        {
            Write-Output "Unfortunately, this password is too weak. Use different password and consider changing your password if you use it somewhere else."
            continue
        }
    }
    catch {
        Write-Output "File for password checks does not exist. Please contact the administrator."
	Write-Output "User creation failed."
        #exit
    }
    
    
    # Checks are all right
    # convert password back to secure string
#    $password = ConvertTo-SecureString $password -AsPlainText
    $wrongPassword = 0

} while ($wrongPassword)


Write-Output "Password $password accepted. Remember it and keep it secret!"

# Create user from group Users
#try {
    New-LocalUser -Name "$name" -Password $password -PasswordNeverExpires -ErrorAction Stop
    Add-LocalGroupMember -Group "$GROUP" -Member "$name" -ErrorAction Stop
#}
#catch {
#    Write-Error -message "Creating account failed."
#    exit
#}

Write-Output "User '$name' successfully created."