function asPrimeData() {
  systemCheckVariableSet AS_TMP_FILE || return 1
  systemCheckVariableSet AS || return 1
  as-describe-auto-scaling-group $AS > $AS_TMP_FILE
  return 0
}

function asCountInstances() {
  systemCheckVariableSet AS_TMP_FILE || return 1
  grep -c ^INSTANCE ${AS_TMP_FILE}
}

function asCountHealthyInstances() {
  systemCheckVariableSet AS_TMP_FILE || return 1
  grep -c InService ${AS_TMP_FILE}
}

function asAllInstancesInElb() {
  # There is a period where an instance is coming online but not yet listed in
  # the ELB. This function determines if we have an instances coming online.
  if [ `asCountInstances` -ne `elbCountInstances` ]; then return 1; fi
}

function asAddInstance() {
  # only add instances if we aren't waiting for one to come online.
  systemCheckVariableSet AS || return 1

  if ! asAllInstancesInElb; then
    as-update-auto-scaling-group $AS --desired-capacity $((asCountInstances + 1))
  fi
}

function asRemoveInstance() {
  systemCheckVariableSet INSTANCE_ID || return 1
  ec2-terminate-instance $INSTANCE_ID
}
