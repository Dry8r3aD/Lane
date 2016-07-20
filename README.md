# Lane
차선 검출

## HSV
RGB값을 사용하여 특정 색깔을 추출 시 빛의 세기에 따라 값의 변동이 심하여 빛의 밝기에 영향이 적은 HSV를 사용.

HSV란 HUE(색상), SATURATION(채도), VALUE(밝기)의 정보로 구성되어 있으며 사람이 인지하는 것과 가장 비슷한 방법입니다.
HUE : 0~360의 범위를 갖고 360도와 0는 같은 색상을 가리킨다.(순환형)
SATURATION : 무채색=0 ~ 가장 진한, 순순한 100
VALUE : 밝기 값, 범위는 0~255 검정색=0, 흰색 =255

## version -1
- 선분들을 벡터로 표현
- 차량의 중심을 화면 중심이라는 정의
- 차선은 차량의 중심이에서 일정거리에 떨어진 선분이라고 정의
- 벡터들의 내적관계를 사용하여 수직으로 만나는 교차점 계산 후 두 점사이의 거리 공식 사용
- 일정 거리안에 있는 선분들을 후보 선분으로 추출 

## version -2
- cvSmooth filter를 사용하여 noise 제거
- canny edge filter를 사용하여 직선 검출 및 rho와 theta를 사용하여 후보점 추출
- ransac algorithm을 사용하여 vanishing point 계산
- vanishing point를 사용하여 inlier 직선들 추출
- inlier 후보 점들을 활용하여 2차 함수 curve fitting  수행
- 차선 표시
