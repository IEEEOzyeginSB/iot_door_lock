import datetime
import peewee

database = peewee.SqliteDatabase("authentication.db")


class Users(peewee.Model):
    card_id = peewee.CharField(primary_key=True)
    student_no = peewee.CharField(null=False)
    name = peewee.CharField(null=False)

    class Meta:
        database = database

    @classmethod
    def is_authorized(cls, id):
        return cls.select().where(Users.card_id == id).exists()

    @classmethod
    def add_user(cls, id, student_no, name):
        try:
            cls.insert(card_id=id, student_no=student_no, name=name).execute()
        except peewee.IntegrityError:
            print("This user already exists in the database!")
	
    @classmethod
    def delete_user(cls, id):
        try:
            cls.delete().where(Users.card_id == id)
        except peewee.IntegrityError:
            print("Non-existant User")

    @classmethod
    def is_empty(cls):
        return cls.select().count() == 0


class Log(peewee.Model):
    user_id = peewee.ForeignKeyField(Users, backref="log")
    date = peewee.DateTimeField()
    log_type = peewee.CharField()

    class Meta:
        database = database

    @classmethod
    def log(cls, id, log_type):
        cls.insert(user_id=id, date=datetime.datetime.now(), log_type=log_type).execute()


if __name__ == "__main__":
    with database:
        database.create_tables([Users, Log])
        Users.add_user("240569743", "S011481", "Doga Yilmaz")
        Users.add_user("2177115659", "S012225", "Berk Buzcu")
