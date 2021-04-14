/*Арсений решил поиграть в настольную игру со своим другом Ильей.
Так как играть обычными кубиками им уже стало неинтересно, Илья
попросил по его чертежам сконструировать новую игральную кость.
Так как он ленивый, то просто накидал точек в пространстве и
сказал,что все они должны лежать в кубике его мечты. У Арсения
есть 3D-принтер, вот только материалы для печати достаточно
дорогие,поэтому он хочет выполнить требования своего друга,
потратив как можно меньше пластика.*/

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <vector>

//структура точки
struct Point {
  long long x;
  long long y;
  long long z;
  size_t id;
  Point(long long x, long long y, long long z, long long id) : x(x), y(y), z(z), id(id) {}
  Point(const Point &other) {
    x = other.x;
    y = other.y;
    z = other.z;
    id = other.id;
  }
  Point &operator=(const Point &other) = default;
};

std::vector<Point> points; // вектор мно-ва точек

Point operator-(const Point &a, const Point &b) {
  return Point(a.x - b.x, a.y - b.y, a.z - b.z, -1);
}

bool operator==(const Point &a, const Point &b) {
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

bool operator<(const Point &a, const Point &b) {
  return (a.z < b.z) || ((a.z == b.z) && (a.y < b.y)) ||
         ((a.z == b.z) && (a.y == b.y) && (a.x < b.x));
}

Point Another_Point(size_t i, size_t j, size_t k) {
  size_t new_id = 0;
  while (new_id == i || new_id == j || new_id == k) {
    ++new_id;
  }
  return points[new_id];
}

//коспаратор сравнения углов
bool Compare_Angle(const Point &a, const Point &b, const Point &p) {
  long long fir = a.z * a.z * ((p.x - b.x) * (p.x - b.x) + (p.y - b.y) * (p.y - b.y));
  long long sec = b.z * b.z * ((p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y));
  return fir < sec;
}
//функция определяющая правосторонность точек
bool Are_Right_Triple(const Point &c, const Point &a, const Point &b) {
  long long det = c.x * a.y * b.z + c.y * a.z * b.x + c.z * a.x * b.y -
           c.z * a.y * b.x - c.y * a.x * b.z - c.x * a.z * b.y;
  return det > 0;
}

// cтруктура отрезока
struct Segment {
  Point start;
  Point finish;
  Segment(const Point &a, const Point &b) : start(a), finish(b) {
    if (b.id < a.id) {
      start = b;
      finish = a;
    }
  }
  Segment(const Segment &other) : start(other.start), finish(other.finish) {}
  size_t Sum() const { return start.id + finish.id; }
  Segment& operator=(const Segment &other) {
    start = other.start;
    finish = other.finish;
    return *this;
  }
};

bool operator<(const Segment &s_1, const Segment &s_2) {
  return (s_1.start.id < s_2.start.id) ||
         ((s_1.start.id == s_2.start.id) && (s_1.finish.id < s_2.finish.id));
}
// cтруктура грани
struct Face {
  Point a;
  Point b;
  Point c;
  Face(const Point &p, const Point &q, const Point &r) : a(p), b(q), c(r) {}
  //коэффициенты плоскости грани: Ax + By + Сz + D = 0
  long long Get_A() const {
    return (b.y - a.y) * (c.z - a.z) - (c.y - a.y) * (b.z - a.z);
  }
  long long Get_B() const {
    return (c.x - a.x) * (b.z - a.z) - (b.x - a.x) * (c.z - a.z);
  }
  long long Get_C() const {
    return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
  }
  long long Get_D() const {
    return -(a.x * this->Get_A() + a.y * this->Get_B() + a.z * this->Get_C());
  }
  size_t Sum() const { return a.id + b.id + c.id; }
  //значение точки, относительно плоскости грани
  long long Get_value(const Point &p) {
    return p.x * this->Get_A() + p.y * this->Get_B() + p.z * this->Get_C() +
           this->Get_D();
  }
};

bool operator==(const Face &f_1, const Face &f_2) {
  return ((f_1.a == f_2.a) && (f_1.b == f_2.b) && (f_1.c == f_2.c));
}

bool operator<(const Face &f_1, const Face &f_2) {
  return (f_1.a.id < f_2.a.id) ||
         ((f_1.a.id == f_2.a.id) && (f_1.b.id < f_2.b.id)) ||
         ((f_1.a.id == f_2.a.id) && (f_1.b.id == f_2.b.id) &&
          (f_1.c.id < f_2.c.id));
}

//функция корректировки индексов грани
Face ClockWise(size_t a, size_t b, size_t c) {
  size_t sum = a + b + c;
  size_t x = std::min(a, std::min(b, c));
  size_t y = std::max(a, std::max(b, c));
  size_t z = sum - x - y;
  if (Are_Right_Triple(Another_Point(x, y, z) - points[x],
                       points[y] - points[x], points[z] - points[x])) {
    return Face(points[x], points[z], points[y]);
  }
  return Face(points[x], points[y], points[z]);
}
//функция возвращающая грань многранника
Face Find_Triangle_On_Hull() {
  size_t p_id = 0; // индекс самой нижней точки
  for (size_t i = 1; i < points.size(); ++i) {
    if (points[i] < points[p_id]) {
      p_id = i;
    }
  }
  long long min_value_of_z = points[p_id].z;
  for (auto& point : points) {
    point.z -= min_value_of_z;
  }
  size_t q_id = 0; // индекс второй точки грани
  while (q_id == p_id) {
    ++q_id;
  }
  for (size_t i = 0; i < points.size(); ++i) {
    if (i == p_id || i == q_id) {
      continue;
    }
    if (Compare_Angle(points[i], points[q_id], points[p_id])) {
      q_id = i;
    }
  }
  size_t r_id = 0; // индекс третьей точки грани
  for (r_id = 0; r_id < points.size(); ++r_id) {
    if (r_id == p_id || r_id == q_id) {
      continue;
    }
    Face face = ClockWise(p_id, q_id, r_id);
    bool at_first_time = true;
    long long value_at_points;
    size_t cnt = 3; // кол-во уже обойдённых точек
    for (size_t i = 0; i < points.size(); ++i) {
      if (i == p_id || i == q_id || i == r_id) {
        continue;
      }
      if (at_first_time) {
        value_at_points = face.Get_value(points[i]);
        at_first_time = false;
        ++cnt;
      } else {
        if (face.Get_value(points[i]) * value_at_points < 0) {
          break;
        }
        ++cnt;
      }
    }
    if (cnt == points.size()) {
      break;
    }
  }
  return ClockWise(p_id, q_id, r_id);
}
//фу-ция наход-ия второй грани отрезка
Face Find_Second_Face(std::stack<Segment> &q,
                      std::map<Segment, std::set<Face>> &seg_map) {
  Segment segment = q.top(); // обрабатываемый отрезок
  size_t third_id = (*(seg_map[segment].begin())).Sum() -
                    segment.Sum(); // индекс третьей точки грани
  size_t new_id = points.size(); // индекс новой точки для новой грани
  for (size_t i = 0; i < points.size(); ++i) {
    if (segment.start.id == i || segment.finish.id == i || i == third_id) {
      continue;
    }
    Face face = ClockWise(i, segment.start.id, segment.finish.id);
    if (new_id != points.size()) {
      if (face.Get_value(points[third_id]) * face.Get_value(points[new_id]) >
          0) {
        new_id = i;
      }
    } else {
      new_id = i;
    }
  }
  Face answer = ClockWise(new_id, segment.start.id, segment.finish.id);
  seg_map[segment].insert(answer);
  q.pop(); // удаляем отрезок из стэка
  seg_map[Segment(points[new_id], segment.start)].insert(answer);
  if (seg_map[Segment(points[new_id], segment.start)].size() < 2) {
    q.push(Segment(points[new_id], segment.start));
  }
  seg_map[Segment(points[new_id], segment.finish)].insert(answer);
  if (seg_map[Segment(points[new_id], segment.finish)].size() < 2) {
    q.push(Segment(points[new_id], segment.finish));
  }
  return answer;
}
// метод заворачивания подарка
std::set<Face> Gift_Wrap() {
  Face face = Find_Triangle_On_Hull(); //первая грань
  std::map<Segment, std::set<Face>> seg_map; // словарь из отрезка и мно-ва её прилигающих граней
  seg_map[Segment(face.a, face.b)].insert(face);
  seg_map[Segment(face.b, face.c)].insert(face);
  seg_map[Segment(face.c, face.a)].insert(face);
  std::stack<Segment> q; // стэк обрабатываемых отрезков
  q.push(Segment(face.a, face.b));
  q.push(Segment(face.b, face.c));
  q.push(Segment(face.a, face.c));
  std::set<Face> set_faces; // мно-во всех граней
  set_faces.insert(face);
  while (!q.empty()) {
    if (seg_map[q.top()].size() < 2) {
      set_faces.insert(Find_Second_Face(q, seg_map));
    } else {
      q.pop();
    }
  }
  return set_faces;
}

int main() {
  size_t m, n;
  std::cin >> m; // кол-во тестов
  while (m-- > 0) {
    std::cin >> n; //кол-во точек
    long long x, y, z;
    for (size_t i = 0; i < n; ++i) {
      std::cin >> x >> y >> z;
      points.push_back(Point(x, y, z, i));
    }
    if (n == 3) {
      std::cout << 1 << '\n' << 3 << " " << 0 << " " << 1 << " " << 2 << '\n';
      continue;
    }
    std::set<Face> set_faces = Gift_Wrap();
    std::cout << set_faces.size() << '\n';
    for (const auto& face : set_faces) {
      std::cout << 3 << " " << face.a.id << " " << face.b.id << " " << face.c.id
                << '\n';
    }
    points.clear();
  }
}
