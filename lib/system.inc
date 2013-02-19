function systemSendEmail() {
  systemError "systemSendEmail is not yet implemented" 
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

function systemGetMeta() {
  if [ -z $1 ]; then return 1; fi
  detail=$1

  var_name=`echo $detail | tr '[:lower:]' '[:upper:]' | sed 's/-/_/g'`
  if [ ! -z ${!var_name} ]; then
    echo ${!var_name}
    return 0
  fi

  if ! systemIsEc2; then return 1; fi

  url="http://169.254.169.254/latest/meta-data/$detail"

  value=`curl --connect-timeout 1 -s $url`
  if [ $? -ne 0 ]; then
    systemError "could not curl the metadata. this should not happen."
    return 1
  fi

  eval ${var_name}=$value
  echo ${!var_name}
}

function systemCleanUp() {
  rm -f $ELB_TMP_FILE $AS_TMP_FILE
  return 0
}

function systemError() {
  if [ -z "$1" ]; then return 1; fi
  echo "${@:2,1}"
  return 0
}

function systemInfo() {
  if [ -z "$1" ]; then return 1; fi

  if [ -z $INFO ]; then return 0; fi
  if ! `$INFO`; then return 0; fi

  echo "${@:2,1}"
  return 0
}

function systemCheckVariableSet() {
  if [ -z "$1" ]; then return 2; fi

  if [ -z ${!1} ]; then
    systemError "You need to set the '$1' variable."
    return 1
  fi
  return 0
}

