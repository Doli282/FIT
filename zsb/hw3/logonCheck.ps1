# Check for antivirus software

# For all Antimalware software
$AVs = @()
try {
    $AVs += Get-CimInstance -Namespace root/SecurityCenter2 -ClassName AntivirusProduct
    Write-Output "AVs fetched"
    
    foreach($AV in $AVs)
    {
        $hex = $('0x{0:x}' -f $AV.productState)

        # Check if Antivir is enabled
        $mid = $hex.Substring(3,2)
        if($mid -match "00|01"){
            Write-Warning -Message "'$($AV.displayName)' seems to be disabled."
        }

        # Check updates
        $end = $hex.Substring(5)
        if ($end -ne "00") {
            Write-Warning -Message "'$($AV.displayName)' seems to be out of date. Check for updates."
        }
    }
}
catch {
    Write-Warning "unable to fetch AVs"
}


# byte mask source:
# https://social.msdn.microsoft.com/Forums/en-US/6501b87e-dda4-4838-93c3-244daa355d7c/wmisecuritycenter2-productstate?forum=vblanguage
# https://jdhitsolutions.com/blog/powershell/5187/get-antivirus-product-status-with-powershell/


# For Windows Defender
try {
    # check if Windows Defender is enabled
    if((Get-MpComputerStatus).AntivirusEnabled -ne $true)
    {
        Write-Warning "Windows Defender Antivirus is not enabled"
    } else 
    {
        Write-Output "WD OK"
    }

    # Update Windows Defender
    Update-MpSignature
}
catch {
    Write-Warning -m "Windows Defender not found."
}


# ---------- Check updates -----------
# check if the Windows Update Service is running
if((Get-Service -Name "wuauserv").Status -ne "Running")
{
    Write-Warning -Message "Windows Update is not running. Please launch Windows Update."
}
else {
    Write-Output "windows updates is running"
}

# Check for uninstalled updates
if ((Get-WUList).Count -gt 0)
{
    Write-Warning -Message "Uninstalled updates available. Please ensure your system is up-to-date."
    # Eventually get the updates automatically
    # Get-WUInstall -Install -AcceptAll -AutoReboot
}
else{
    Write-Output "updates OK"
}
Read-Host