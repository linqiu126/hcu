<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusyspmglobaldatainfo */

$this->title = $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcusyspmglobaldatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusyspmglobaldatainfo-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->sid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->sid], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
            'sid',
            'taskhcuvmec',
            'taskhwinvec',
            'tasksyspmec',
            'taskmodbusec',
            'taskcloudvelaec',
            'taskavorionec',
            'taskspsvirgoec',
            'taskhsmmpec',
            'taskemcec',
            'taskpm25ec',
            'taskwinddirec',
            'taskwindspdec',
            'tasktempec',
            'taskhumidec',
            'tasknoiseec',
            'taskairprsec',
            'taskco1ec',
            'tasklightstrec',
            'taskalcoholec',
            'taskhchoec',
            'tasktoxicgasec',
            'restartcnt',
            'cloudvelaconncnt',
            'cloudveladisccnt',
            'clouddatatimeoutcnt:datetime',
            'timestamp:datetime',
        ],
    ]) ?>

</div>
