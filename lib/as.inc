function asPrimeData() {
  systemCheckVariableSet AS_TMP_FILE || return 1
  systemCheckVariableSet AS || return 1
  as-describe-auto-scaling-groups $AS > $AS_TMP_FILE

  if egrep '^No AutoScalingGroups found' $AS_TMP_FILE; then
    systemInfo "Could not obtain details for AutoScalingGroup '$AS'."
    return 1
  fi
}

function asMember() {
  systemCheckVariableSet INSTANCE_ID || return 1
  systemCheckVariableSet AS_TMP_FILE || return 1
  systemCheckVariableSet AS || return 1
  egrep "^INSTANCE\ +$INSTANCE_ID\ +.*" ${AS_TMP_FILE} &> /dev/null

  if [ $? -ne 0 ]; then
    systemInfo "Instance '$INSTANCE_ID' is not in the AutoScalingGroup '$AS'."
    return 1
  fi

  return 0
}

function asCountInstances() {
  systemCheckVariableSet AS_TMP_FILE || return 1
  grep -c ^INSTANCE ${AS_TMP_FILE}
}

function asAddInstance() {
  # only add instances if we aren't waiting for one to come online.
  systemCheckVariableSet AS || return 1
  systemCheckVariableSet DRY_RUN || return 1

  if [ -z $SPUN_NEW_INSTANCE ]; then
    SPUN_NEW_INSTANCE='true'
    systemInfo "Increasing capacity of $AS to $((asCountInstances +1))."
    $DRY_RUN || as-update-auto-scaling-group $AS --desired-capacity $((asCountInstances + 1))
    return 0
  else
    return 1
  fi
}

function asRemoveInstance() {
  systemCheckVariableSet INSTANCE_ID || return 1
  systemCheckVariableSet DRY_RUN || return 1
  systemInfo "Terminating instance '$INSTANCE_ID'."
  $DRY_RUN || poweroff +1 -p
}
