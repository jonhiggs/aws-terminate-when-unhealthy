function elbPrimeData() {
  systemCheckVariableSet ELB_TMP_FILE || return 1
  systemCheckVariableSet ELB || return 1
  elb-describe-instance-health $ELB > $ELB_TMP_FILE || systemSendEmail "ELB Details" "Failed to get the ELB details."
  return 0
}

function elbMember() {
  systemCheckVariableSet INSTANCE_ID || return 1
  systemCheckVariableSet ELB_TMP_FILE || return 1
  systemCheckVariableSet ELB || return 1
  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${ELB_TMP_FILE} &> /dev/null

  if [ $? -ne 0 ]; then
    systemInfo "Instance '$INSTANCE_ID' is not in the ELB '$ELB'."
    return 1
  fi

  return 0
}

function elbOutOfService() {
  # elb thinks the instance is out of service. This doesn't mean we will shut
  # it down just yet. That is the job of serviceOutOfService.
  systemCheckVariableSet INSTANCE_ID || return 1
  systemCheckVariableSet ELB_TMP_FILE || return 1
  systemCheckVariableSet ELB || return 1

  if [ -z ${INSTANCE_ID} ]; then
    systemError "I don't know what the INSTANCE_ID is."
    return 1
  fi

  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${ELB_TMP_FILE} | awk '{ print $3 }' | grep "^OutOfService$" &> /dev/null

  if [ $? -eq 0 ]; then
    systemInfo "$INSTANCE_ID in $ELB is OutOfService."
    return 0
  else
    systemInfo "$INSTANCE_ID in $ELB is InService."
    return 1
  fi
}

function elbCountInstances() {
  systemCheckVariableSet ELB_TMP_FILE || return 1
  wc -l < ${ELB_TMP_FILE}
}

function elbCountHealthyInstances() {
  systemCheckVariableSet ELB_TMP_FILE || return 1
  grep -c InService ${ELB_TMP_FILE}
}

function elbCountInstancesComingOnline() {
  $(( `elbCountInstances` - `elbCountHealthyInstances` ))
}
