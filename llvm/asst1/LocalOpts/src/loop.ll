; ModuleID = 'loop.c'
source_filename = "loop.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @compute(i32, i32) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  store i32 0, i32* %5, align 4
  %6 = load i32, i32* %3, align 4
  %7 = mul nsw i32 %6, 2
  %8 = load i32, i32* %5, align 4
  %9 = add nsw i32 %8, %7
  store i32 %9, i32* %5, align 4
  %10 = load i32, i32* %3, align 4
  %11 = mul nsw i32 %10, 3
  %12 = load i32, i32* %5, align 4
  %13 = add nsw i32 %12, %11
  store i32 %13, i32* %5, align 4
  %14 = load i32, i32* %3, align 4
  %15 = mul nsw i32 %14, 8
  %16 = load i32, i32* %5, align 4
  %17 = add nsw i32 %16, %15
  store i32 %17, i32* %5, align 4
  %18 = load i32, i32* %4, align 4
  %19 = sdiv i32 %18, 2
  %20 = load i32, i32* %5, align 4
  %21 = sub nsw i32 %20, %19
  store i32 %21, i32* %5, align 4
  %22 = load i32, i32* %4, align 4
  %23 = sdiv i32 %22, 4
  %24 = load i32, i32* %5, align 4
  %25 = sub nsw i32 %24, %23
  store i32 %25, i32* %5, align 4
  %26 = load i32, i32* %4, align 4
  %27 = sdiv i32 %26, 8
  %28 = load i32, i32* %5, align 4
  %29 = sub nsw i32 %28, %27
  store i32 %29, i32* %5, align 4
  %30 = load i32, i32* %5, align 4
  ret i32 %30
}

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [3 x i32] [i32 10, i32 15, i32 6]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 11.0.3 (clang-1103.0.32.62)"}
