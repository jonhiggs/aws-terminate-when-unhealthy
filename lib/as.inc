function asCountInstances() {
  grep -c ^INSTANCE ${AS_TMP_FILE}
}

function asCountHealthyInstances() {
  grep -c InService ${AS_TMP_FILE}
}

function asCountInstancesComingOnline() {
  $(( `elbCountInstances` - `elbCountHealthyInstances` ))
}
