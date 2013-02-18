function elbPrimeData() {
  # not done
  return 1
}

function elbMember() {
  if [ -z ${INSTANCE_ID} ]; then return 1; fi

  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${TMP_FILE} &> /dev/null
  return $?
}

function elbOutOfService() {
  if [ -z ${INSTANCE_ID} ]; then return 1; fi

  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${TMP_FILE} | awk '{ print $3 }' | grep "^OutOfService$" &> /dev/null
  return $? 
}
