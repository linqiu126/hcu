<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcusyspmglobaldatainfo */

$this->title = 'Create Hcusyspmglobaldatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcusyspmglobaldatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusyspmglobaldatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
