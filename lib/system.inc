function systemEmailEnabled() {
  ! [ -z $EMAIL_RCPT ]
  return $?
}

function systemSendEmail() {
  if ! systemEmailEnabled; then
    systemInfo "Not sending email because we don't have an address."
    return 0
  fi

  if [ -z "$2" ]; then
    systemInfo "Not sending email because we don't have content."
    return 1
  fi

  subject="aws-terminate-when-unhealthy: $1"

  content_header="Instance '$INSTANCE_ID' has been out of service for $((`date +%s` - $START_TIME)) seconds."
  content=$2

  content_file=`mktemp -t XXXXX`
  echo $content_header > $content_file
  echo $content >> $content_file

  systemInfo "Sending email - $1"

  systemDebug "Emailing:"
  systemDebug "To: $EMAIL_RCPT"
  systemDebug "Subject: $subject"
  systemDebug "Content: $content"

  $DRY_RUN || mail -s "$subject" $EMAIL_RCPT < $content_file
  rm $content_file
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
  systemRemoveLock
  return 0
}

function systemError() {
  if [ -z "$1" ]; then return 1; fi
  echo "${@:2,1}" 1>&2;
  return 0
}

function systemInfo() {
  systemCheckVariableSet INFO_FILE || return 1

  if [ -z "$1" ]; then return 1; fi

  if [ -z $INFO ]; then return 0; fi
  if ! `$INFO`; then return 0; fi

  MSG="${@:2,1}"
  echo $MSG
  echo "`date` - $MSG" >> $INFO_FILE
  return 0
}

function systemDebug() {
  systemCheckVariableSet DEBUG_FILE || return 1

  if [ -z "$1" ]; then return 1; fi

  if [ -z $DEBUG ]; then return 0; fi
  if ! `$DEBUG`; then return 0; fi

  MSG="${@:2,1}"
  echo $MSG
  echo "`date` - $MSG" >> $DEBUG_FILE
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

function systemCreateLock() {
  systemCheckVariableSet LOCK_FILE || return 1

  if systemLockedTooLong; then
    systemInfo "Removing lock because it's been around too long."
    # TODO: kill old process
    systemRemoveLock
  fi

  if systemLocked; then
    systemInfo "Cannot lock because there is a process still running."
    return 1
  fi

  systemInfo "Lockfile Created."
  touch $LOCK_FILE
  return 0
}

function systemRemoveLock() {
  systemCheckVariableSet LOCK_FILE || return 1

  rm -f $LOCK_FILE

  if [ -f $LOCK_FILE ]; then
    systemError "Failed to remove lock file '$LOCK_FILE' for an unknown reason."
    return 1
  else
    systemInfo "Lockfile '$LOCK_FILE' Removed."
    return 0
  fi
}

function systemLocked() {
  systemCheckVariableSet LOCK_FILE || return 1

  if [ -f $LOCK_FILE ]; then
    systemInfo "Script locked."
    return 0
  else
    systemInfo "Script unlocked."
    return 1
  fi
}

function systemLockedTooLong() {
  if ! systemLocked; then return 1; fi

  systemCheckVariableSet START_TIME || return 1
  systemCheckVariableSet OUT_OF_SERVICE_THRESHOLD || return 1
  systemCheckVariableSet LOCK_FILE || return 1

  if [ `uname` == "Darwin" ]; then
    lock_creation_time=`/usr/bin/stat -f '%m' $LOCK_FILE`
  else
    lock_creation_time=`stat --format=%Y $LOCK_FILE`
  fi

  lock_max_age=$(( $OUT_OF_SERVICE_THRESHOLD * 2 ))
  lock_min_creation_time=$(( $START_TIME - $lock_max_age ))

  systemDebug "Out Of Service Threshold: $OUT_OF_SERVICE_THRESHOLD"
  systemDebug "Lock Max Age:             $(($START_TIME - $lock_max_age - $START_TIME))"
  systemDebug "Lock Creation Time:       $(($lock_creation_time - $START_TIME))"
  systemDebug "Lock Min Creation Time:   $(($lock_min_creation_time - $START_TIME + $lock_max_age))"
  systemDebug "Start Time:               $(($START_TIME - $START_TIME + $lock_max_age))"

  [ $lock_creation_time -lt $lock_min_creation_time ]
  if [ $? -eq 0 ]; then
    systemInfo "Lock is too old"
    return 0
  else
    return 1
  fi
}

function systemBootComplete() {
  systemCheckVariableSet BOOTED || return 1
  $BOOTED
  return $?
}
