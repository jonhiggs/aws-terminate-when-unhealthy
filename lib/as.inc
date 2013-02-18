function asCountInstances() {
  grep -c ^INSTANCE ${AS_TMP_FILE}
}

function asCountHealthyInstances() {
  grep -c InService ${AS_TMP_FILE}
}

function asAllInstancesInElb() {
  # There is a period where an instance is coming online but not yet listed in
  # the ELB. This function determines if we have an instances coming online.
  if [ `asCountInstances` -ne `elbCountInstances` ]; then return 1; fi
}

function asIncrementCapacity() {
  # only add instances if we aren't waiting for one to come online.
  if ! asAllInstancesInElb; then
    as-update-auto-scaling-group $AS --desired-capacity $((asCountInstances + 1))
  fi
}
