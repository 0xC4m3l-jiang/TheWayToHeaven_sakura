; ModuleID = 'nopt_loop.ll'
source_filename = "../test/loop.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.15.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @loop(i32 %0, i32 %1, i32 %2) #0 {
  %4 = add nsw i32 %1, 2
  br label %5

5:                                                ; preds = %7, %3
  %.0 = phi i32 [ 0, %3 ], [ %6, %7 ]
  %6 = add nsw i32 %.0, 1
  br label %7

7:                                                ; preds = %5
  %8 = icmp slt i32 %6, %0
  br i1 %8, label %5, label %9

9:                                                ; preds = %7
  %10 = add nsw i32 0, %4
  ret i32 %10
}

; Function Attrs: noinline nounwind ssp uwtable
define void @looper(i32 %0, i32 %1, i32 %2, i32 %3) #0 {
  %5 = add nsw i32 %1, 2
  %6 = icmp ne i32 %5, 0
  br label %7

7:                                                ; preds = %14, %4
  %.0 = phi i32 [ 0, %4 ], [ %.1, %14 ]
  %8 = add nsw i32 %.0, 1
  br label %9

9:                                                ; preds = %10, %7
  %.1 = phi i32 [ %8, %7 ], [ %12, %10 ]
  br i1 %6, label %10, label %13

10:                                               ; preds = %9
  %11 = add nsw i32 %0, 2
  %12 = add nsw i32 %.1, 1
  br label %9

13:                                               ; preds = %9
  br label %14

14:                                               ; preds = %13
  %15 = icmp slt i32 %.1, %0
  br i1 %15, label %7, label %16

16:                                               ; preds = %14
  ret void
}

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [3 x i32] [i32 10, i32 15, i32 6]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 11.0.3 (clang-1103.0.32.62)"}
