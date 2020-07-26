; ModuleID = '../tests/avail_expr.c'
source_filename = "../tests/avail_expr.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main(i32, i8**) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  store i32 50, i32* %6, align 4
  %12 = load i32, i32* %4, align 4
  %13 = load i32, i32* %6, align 4
  %14 = add nsw i32 %12, %13
  store i32 %14, i32* %7, align 4
  store i32 96, i32* %8, align 4
  %15 = load i32, i32* %7, align 4
  %16 = load i32, i32* %8, align 4
  %17 = add nsw i32 %15, %16
  store i32 %17, i32* %10, align 4
  %18 = load i32, i32* %6, align 4
  %19 = load i32, i32* %7, align 4
  %20 = icmp slt i32 %18, %19
  br i1 %20, label %21, label %28

21:                                               ; preds = %2
  %22 = load i32, i32* %7, align 4
  %23 = load i32, i32* %6, align 4
  %24 = sub nsw i32 %22, %23
  store i32 %24, i32* %11, align 4
  %25 = load i32, i32* %8, align 4
  %26 = load i32, i32* %7, align 4
  %27 = mul nsw i32 %25, %26
  store i32 %27, i32* %10, align 4
  br label %35

28:                                               ; preds = %2
  %29 = load i32, i32* %7, align 4
  %30 = load i32, i32* %6, align 4
  %31 = add nsw i32 %29, %30
  store i32 %31, i32* %11, align 4
  %32 = load i32, i32* %8, align 4
  %33 = load i32, i32* %7, align 4
  %34 = mul nsw i32 %32, %33
  store i32 %34, i32* %10, align 4
  br label %35

35:                                               ; preds = %28, %21
  %36 = load i32, i32* %6, align 4
  %37 = load i32, i32* %8, align 4
  %38 = sub nsw i32 %36, %37
  store i32 %38, i32* %7, align 4
  %39 = load i32, i32* %7, align 4
  %40 = load i32, i32* %11, align 4
  %41 = add nsw i32 %39, %40
  store i32 %41, i32* %9, align 4
  ret i32 0
}

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [3 x i32] [i32 10, i32 15, i32 6]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 11.0.3 (clang-1103.0.32.62)"}
