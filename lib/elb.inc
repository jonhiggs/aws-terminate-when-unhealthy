function elbMember() {
  if [ -z ${INSTANCE_ID} ]; then return 1; fi

  egrep "^INSTANCE_ID\ +$INSTANCE_ID\ +.*" ${TMP_FILE}
  return $?
}

function elbOutOfService() {
  if [ `elb-describe-instance-health layar --instances ${INSTANCE_ID} 2> /dev/null | awk '{ print $3 }' | grep -c "^OutOfService$"` -eq 1 ]; then
    return 0
  fi

  return 1
}
