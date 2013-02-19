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
  if [ -z $1]; then return 1; fi
  if ! systemIsEc2; then return 1; fi

  detail=$1

  var_name=`echo $detail | tr '[:lower:]' '[:upper:]' | sed 's/-/_/g'`
  if [ ! -z ${!var_name} ]; then
    echo ${!var_name}
    return 0
  fi


  url="http://169.254.169.254/latest/meta-data/$detail"


  value=`curl --connect-timeout 1 -s $url`
  if [ $? -ne 0 ]; then
    echo "could not curl the metadata. this should not happen."
    return 1
  fi

  eval ${var_name}=$value
  echo ${!var_name}
}

function systemGetInstanceId(){
  if [ -z $INSTANCE_ID ]; then
    INSTANCE_ID=`curl --connect-timeout 1 -s http://169.254.169.254/latest/meta-data/instance-id`
    if [ $? -ne 28 ]; then return 1; fi
  fi
  echo $INSTANCE_ID
  return 0
}
