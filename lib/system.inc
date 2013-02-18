function systemSendEmail() {
  echo "not implemented" 
  return 1
}

function systemGetInstanceId(){
  if [ -z $INSTANCE_ID ]; then
    INSTANCE_ID=`curl --connect-timeout 1 -s http://169.254.169.254/latest/meta-data/instance-id`
    if [ $? -eq 28 ]; then return 1; fi
  fi
  echo $INSTANCE_ID
  return 0
}
