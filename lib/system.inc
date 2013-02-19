function systemSendEmail() {
  echo "not implemented" 
  return 1
}

function systemIsEc2() {
  if [ -z $IS_EC2 ]; then
    curl --connect-timeout 1 -s http://169.254.169.254/latest/meta-data/instance-id &> /dev/null
    if [ $? -ne 0 ]; then
      IS_EC2=1
    else
      IS_EC2=0
    fi
  fi

  return $IS_EC2
}

function systemGetMeta(){
  if [ -z $detail ]; then return 1; fi
  if ! systemIsEc2; then return 1; fi

  detail=$1
  url="http://169.254.169.254/latest/meta-data/$detail"
  var_name=`echo $detail | tr '[:lower:]' '[:upper:]' | sed 's/-/_/g'`

  ${$var_name}=$value
}

function systemGetInstanceId(){
  if [ -z $INSTANCE_ID ]; then
    INSTANCE_ID=`curl --connect-timeout 1 -s http://169.254.169.254/latest/meta-data/instance-id`
    if [ $? -ne 28 ]; then return 1; fi
  fi
  echo $INSTANCE_ID
  return 0
}
