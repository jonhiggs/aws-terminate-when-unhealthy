function elbPrimeData() {
  if [ -z $TMP_FILE ]; then return 1; fi
  if [ -z $ELB ]; then return 1; fi
  elb-describe-instance-health $ELB > $TMP_FILE
  return 0
}

function elbMember() {
  if [ -z ${INSTANCE_ID} ]; then return 1; fi

  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${TMP_FILE} &> /dev/null
  return $?
}

function elbOutOfService() {
  # elb thinks the instance is out of service. This doesn't mean we will shut
  # it down just yet. That is the job of serviceOutOfService.

  if [ -z ${INSTANCE_ID} ]; then return 1; fi
  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${TMP_FILE} | awk '{ print $3 }' | grep "^OutOfService$" &> /dev/null
  return $? 
}

function elbCountInstances() {
  wc -l < ${ELB_TMP_FILE}
}

function elbCountHealthyInstances() {
  grep -c InService ${ELB_TMP_FILE}
}

function elbCountInstancesComingOnline() {
  $(( `elbCountInstances` - `elbCountHealthyInstances` ))
}
