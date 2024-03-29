ესემბლის ემულატორი:

*mainEmulator.cpp ფაილიდან ეშვება პროგრამა, რომელიც მუშაობისას იყენებს 1 კლასს და 2 .h ფაილს. Emulator 
კლასში ყველა ინსტრუქციის უშუალოდ შესრულება ხდება (brench, alu, load/store, jump/call). რადგანაც ჩემი კომპიუტერი
და არა მხოლოდ ჩემი, 64 bit იანია, ანუ pointers მეხსიერება არის 8 byte, emulator კლასში დავამატე ოთხი load/store
ინსტრუქცია (lwlong, swlong, lblong, sblong), რომლებიც მუშაობენ 8 ბაიტიან მეხსიერებასთან, მათი მეშვეობით უკვე ჩემი ტესტები
უფრო ფართოა და მოიცავს ფოინთერების ცნებებსაც (სხვაგვარად ეს რთული იქნებოდა).

*TestHandler.h ფაილში ინტერფეისშიც კარგად ჩანს მისი დანიშნულება, ის კონკრეტული ტესტებისთვის assembly ფაილში სეტავს 
არგუმენტებს და შემდეგ ამავე ფაილის მეშვეობით ტესტის შედეგი იბეჭდება. 

*Helper.h ფაილში აღწერილია თითქმის ყველა ის დამხმარე ფუნქცია რომელიც მუშაობისას დამჭირდა (ისეთივე მარტივებიც 
როგორებიცაა stringToInt და ა.შ), ეს ცალკე h, cpp ფაილში გავიტანე რათა კოდი არ გადატვირთულიყო.

*პირობაში მოცემული იყო, რომ კარგი იქნებოდა თუ ჩავაშენებდით ზოგ ფუნქციას მარტივი იმპლემენტაციებით. strlen, malloc
realloc და free კარგად ჩანს კოდიდანაც რომ თითო არგუმენტს იღებენ (realloc 2 ს) და მათი დაწერა არ ყოფილა რთული,
რაც შეეხება printf ს, აქ გამოვიყენე ასეთი მიდგომა: რადგან არ ვიცით ზუსტად რამდენი არგუმენტი გადმოგვეცემა არგუმენტებს შორის 
პირველი უნდა იყოს დანარჩენი არგუმენტების რაოდენობა.

*რაც შეეხება ტესტებს, ყველა მაქვს .txt ფაილში დაწერილი. mainEmulator.cpp კითხულობს ხაზ ხაზად და ყველა ინსტრუქციას ზედმიწევნით
ასრულებს. ყველა გამოძახებული ფუნქცია ra(return address) ს დახმარებით ბრუნდება  მის გამოძახების ადგილთან, გამონაკლისია ბოლო 
ret ინსტრუქცია main ში, რომელსაც ხელოვნურად ვუსეტავთ ra = -1 ს და ამის მიხედვით წყვეტს ჩვენი ტესტი მუშაობას. 

*ისევე როგორც სტანდარტულ კომპიუტერებზე, emulator საც აქვს 64 რეგისტრი, და stack ს მივანიჭე 1024 ბაიტი - რომელიც გადავსების
შემთხვევაში აბრუნებს ერორს (Stack Overflow). Stack ის მენეჯმენტისთვის ის წარმოდგენილია როგორც unsigned char ების მასივი რათა 
მარტივად განვათავსოთ მასში როგორც byte, hbyte, wbyte და longbyte. და ასევე უფრო ამარტივებს იმ სიტუაციას, როცა მომხმარებელი
sp ს არა ოთხის ჯერადი byte თ ამენეჯმენტებს.

*უფრო ფართოდ არგუმენტების დასეტვაზე: კოდს როცა შეხედავთ, ნახავთ რომ წინასწარ ვუსეტავ ყველა ტესტს არგუმენტს initTestArgumets 
ფუნქციით. რადგანაც გვიწევს მეხსიერებასთან მუშაობა, ანუ თუ მაგალითად ტესტს უნდა დავუსეტო {1,2,3}ვექტორი ეს ვექტორი მჭირდება რომ
ტესტის მორჩენამდე აუცილებლად არსებობდეს და მის მეხსიერებას რამე არ გადაეწეროს, სწორედ ამიტომ, TestHandler.cpp ში არგუმენტების
დასეტვისას ეს არგუმენტები მათ გადაეცემათ, წინააღმდეგ შემთხვევაში (ანუ TestHandler.cpp ს ფუნქციაშივე რომ შემექმნა ეს არგუმენტები და 
ისე გამეყოლებინა) ფუნქციის მუშაობის დასრულებისთანავე გადაეწერებოდა თავზე არგუმენტს მეხსიერება და ესემბლი.txt ფაილში დასეტილი 
long long მისამართი აღარ იქნებოდა ვალიდური.

დანარჩენი პატარ-პატარა უამრავი დეტალი მაქსიმალურადაა აღწერილი კომენტარებში. 
p.s ტესტის გასაშვებად აუცილებელია ყველა ფაილის მონიშვნა.