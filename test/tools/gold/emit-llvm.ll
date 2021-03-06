; RUN: llvm-as %s -o %t.o

; RUN: ld -plugin %llvmshlibdir/LLVMgold.so \
; RUN:    --plugin-opt=emit-llvm \
; RUN:    --plugin-opt=generate-api-file \
; RUN:    -shared %t.o -o %t2.o
; RUN: llvm-dis %t2.o -o - | FileCheck %s
; RUN: FileCheck --check-prefix=API %s < %T/../apifile.txt

; RUN: ld -plugin %llvmshlibdir/LLVMgold.so \
; RUN:     -m elf_x86_64 --plugin-opt=save-temps \
; RUN:    -shared %t.o -o %t3.o
; RUN: llvm-dis %t3.o.bc -o - | FileCheck %s
; RUN: llvm-dis %t3.o.opt.bc -o - | FileCheck --check-prefix=OPT %s

target triple = "x86_64-unknown-linux-gnu"

; CHECK: define internal void @f1()
; OPT-NOT: @f1
define hidden void @f1() {
  ret void
}

; CHECK: define hidden void @f2()
; OPT: define hidden void @f2()
define hidden void @f2() {
  ret void
}

@llvm.used = appending global [1 x i8*] [ i8* bitcast (void ()* @f2 to i8*)]

; CHECK: define void @f3()
; OPT: define void @f3()
define void @f3() {
  call void @f4()
  ret void
}

; CHECK: define internal void @f4()
; OPT-NOT: @f4
define linkonce_odr void @f4() {
  ret void
}

; CHECK: define linkonce_odr void @f5()
; OPT: define linkonce_odr void @f5()
define linkonce_odr void @f5() {
  ret void
}
@g5 = global void()* @f5

; CHECK: define internal void @f6() unnamed_addr
; OPT: define internal void @f6() unnamed_addr
define linkonce_odr void @f6() unnamed_addr {
  ret void
}
@g6 = global void()* @f6


; API: f1 PREVAILING_DEF_IRONLY
; API: f2 PREVAILING_DEF_IRONLY
; API: f3 PREVAILING_DEF_IRONLY_EXP
; API: f4 PREVAILING_DEF_IRONLY_EXP
; API: f5 PREVAILING_DEF_IRONLY_EXP
; API: f6 PREVAILING_DEF_IRONLY_EXP
; API: g5 PREVAILING_DEF_IRONLY_EXP
; API: g6 PREVAILING_DEF_IRONLY_EXP
