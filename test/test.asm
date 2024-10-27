noop
pub, test, 0xFC
srb, R0, 0, $0:test
srb, R0, 1, $1:test
srb, R0, 2, $2:test
srb, R0, 3, $3:test
srb, R0, 4, $4:test
srb, R0, 5, $5:test
srb, R0, 6, $6:test
srb, R0, 7, $7:test
cpr, CA, R0
cpr, CB, CID
upd
cpr, R2, CRADD
hlt
